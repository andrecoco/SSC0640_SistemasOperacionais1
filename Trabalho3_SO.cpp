#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "Memoria.h"
#include "Processo.h"
#include "Auxiliar.h"
using namespace std;

//DEFINES
#define ALG_SUBSTITUICAO "RELOGIO" //LRU ou RELOGIO


/*
    Ex: TAM_PAGINA 1024
        TAM_MEMORIA_SECUNDARIA 32 * TAM_PAGINA
        tamanho total = 32,768 (dá pra representar com 15 bits)
        como a pagina tem 1024 endereços (representados com 10 bits), temos
                  endereço (15 bits): 00101 1001011010
                  n_pagina (5 bits) : 00101
        endereço na pagina(10 bits) :       1001011010

        Com isso só pelo endereço dá pra saber o número da página e o endereço na página :)
*/

/* OBS:
A gente pode definir os metodos dentro da classe, ou fora.
Ex:
class nomeClasse{
    void nomeMetodo(){
        //
    }
}

Ou dá pra fazer:
class nomeClasse{
    void nomeMetodo();
}
void nomeClasse::nomeMetodo(){
        //
    }
*/

/* OBS2: (Se quiser manter o msm estilo de nome das coisas)
Eu usei tudo maiusculo pros defines -> NOME_DEFINE
Pras classes eu coloquei a primeira letra maiscula -> NomeClasse
Pras variaveis eu usei -> nomeVar
*/

class Emulador { //seria o SO, todos os comandos passam por essa classe, ela distribui o resto
    bool useRelogio; //define se é relógio ou LRU
    list<Processo> processos;
    MemoriaPrincipal memPrincipal;
    MemoriaSecundaria memSecundaria;
    int numBitsEnderecoLogico;
    int numBitsIDPagina;

    //Coisas para algoritmo de substituição
    list<pair<int, int>> paginasAlocadasNaRAM; //par ID do processo e ID da pagina na memVirtual
    list<pair<int, int>>::iterator ponteiro; //pra utilizar no relógio, aponta para a página mais antiga

public:
    Emulador() {

        //Aloca as memorias
        memPrincipal = *(new MemoriaPrincipal);
        memSecundaria = *(new MemoriaSecundaria);

        if (ALG_SUBSTITUICAO == "RELOGIO") {
            useRelogio = true;
        }
        else {
            useRelogio = false;
        }

        paginasAlocadasNaRAM = *(new list<pair<int, int>>);

        ponteiro = paginasAlocadasNaRAM.end();

        //o endereco é dividido em dois pedaços, o primeiro é o ID da página, o resto é o endereço na página
        numBitsEnderecoLogico = (int)ceil(log2(TAM_MEMORIA_SECUNDARIA));
        numBitsIDPagina = numBitsEnderecoLogico - (int)ceil(log2(TAM_PAGINA));

        cout << "############ EMULADOR ############" << endl;
        cout << "O endereco logico possui " << numBitsEnderecoLogico << "bits" << endl;
        cout << "Os primeiros " << numBitsIDPagina << "bits representam o numDaPagina" << endl;

        //cout << numBitsEnderecoLogico << " " << numBitsIDPagina << endl;
        //cout << binary2Decimal("100111") << endl;
    }

    //Retira uma pagina da RAM a partir de um algoritmo de substituicao
    int algSubs() {
        int IDMoldura = -1; //ID da moldura a ser liberada
        int IDPaginaVirtual; //ID da pagina virtual
        int PID; //ID do processo cuja pagina sera retirada da moldura
        list<Processo>::iterator itr;
        list<pair<int, int>>::iterator itrLista;

        if (useRelogio) { //Algoritmo do Relogio
            bool achou = false;
            while (!achou) {
                PID = ponteiro->first;
                IDPaginaVirtual = ponteiro->second;

                //pega a tabela de paginas do processo e confere o bit R
                for (itr = processos.begin(); itr != processos.end(); itr++) {
                    if (itr->id == PID) {
                        break;
                    }
                }
                
                for (int i = 0; i < (int)itr->tabelaPaginas.size(); i++) {
                    if (itr->tabelaPaginas[i].IDPagina == IDPaginaVirtual) {
                        if (itr->tabelaPaginas[i].R) { //se o bit R tiver setado, coloca 0 e avanca o ponteiro
                            itr->tabelaPaginas[i].R = false;
                            
                            ponteiro++;

                            //simula o comportamento de uma lista circular
                            if (ponteiro == paginasAlocadasNaRAM.end()) {
                                ponteiro = paginasAlocadasNaRAM.begin();
                            }
                        }
                        else {
                            achou = true;
                        }
                        break;
                    }
                }

            }
            
        }
        else { //LRU
            //a pagina menos recentemente usada estará no inicio da lista
            PID = paginasAlocadasNaRAM.front().first;
            IDPaginaVirtual = paginasAlocadasNaRAM.front().second;
        }
        
        //pega a tabela de paginas do processo e recupera o numero da moldura dessa pagina
        for (itr = processos.begin(); itr != processos.end(); itr++) {
            if (itr->id == PID) {
                break;
            }
        }

        for (int i = 0; i < (int)itr->tabelaPaginas.size(); i++) {
            if (itr->tabelaPaginas[i].IDPagina == IDPaginaVirtual) {
                IDMoldura = itr->tabelaPaginas[i].numMoldura;
                    
                //Verifica se a pagina foi modificada enquanto em RAM
                if (itr->tabelaPaginas[i].M) {
                    //atualiza a copia na memoria secundaria
                    memSecundaria.atualizaPagina(IDPaginaVirtual, memPrincipal.retornaPagina(IDMoldura), true);
                }
                break;
            }
        }

        if (useRelogio) {
            //confere se o ponteiro precisa ser alterado
            if (paginasAlocadasNaRAM.size() == 1) { //se a lista for ficar vazia
                ponteiro = paginasAlocadasNaRAM.end();
            }
            else if (ponteiro->second == IDPaginaVirtual) {

                ponteiro++;

                //simula o comportamento de uma lista circular
                if (ponteiro == paginasAlocadasNaRAM.end()) {
                    ponteiro = paginasAlocadasNaRAM.begin();
                }
            }
        }

        //retira da lista de paginas na RAM
        for (itrLista = paginasAlocadasNaRAM.begin(); itrLista != paginasAlocadasNaRAM.end(); itrLista++) {
            if (itrLista->second == IDPaginaVirtual) {
                paginasAlocadasNaRAM.erase(itrLista);
                break;
            }
        } 

        //Libera essa moldura da RAM
        cout << "Emulador: O algoritmo de substituicao liberou a moldura " << IDMoldura << endl;
        memPrincipal.desalocarPagina(IDMoldura);
        
        return 0;
    }

    int alocaPaginaRAM(int PID, string endereco) {
        /*
        Tabela de Retorno
        -1 - Algum erro
        0 - Conseguiu Alocar
        1 - Conseguiu Alocar mas precisou tirar uma da RAM
        */
        int IDPaginaVirtual = -1;
        int NumPaginaNoProcesso = binary2Decimal(endereco.substr(0, numBitsIDPagina)); //o numero da pagina sao os numBitsIDPagina primeiros bits
        list<Processo>::iterator itr;
                                                                                           
        //pega a tabela de paginas do processo
        for (itr = processos.begin(); itr != processos.end(); itr++) {
            if (itr->id == PID) {
                //pega o ID da pagina virtual
                IDPaginaVirtual = itr->retornaIDPagina(NumPaginaNoProcesso);
                break;
            }
        }

        if (IDPaginaVirtual == -1) {
            cout << "Emulador: Nao foi possivel encontrar a pagina!" << endl;
            return -1;
        }
        
        Pagina paginaVirtual = memSecundaria.retornaPaginaPeloID(IDPaginaVirtual);
        
        int numMoldura = memPrincipal.alocarPagina(paginaVirtual);
        
        if (numMoldura == -1) { //verifica se conseguiu alocar
            //se a RAM esta cheia, desaloca uma pagina
            algSubs();
            numMoldura = memPrincipal.alocarPagina(paginaVirtual);
        }

        //ajeita a tabela do processo
        itr->tabelaPaginas[NumPaginaNoProcesso].presente = true;
        itr->tabelaPaginas[NumPaginaNoProcesso].numMoldura = numMoldura;

        //adiciona na lista de paginas na RAM
        paginasAlocadasNaRAM.push_back(pair<int, int>(itr->id, IDPaginaVirtual));

        //Se a RAM estivesse vazia, o ponteiro estará errado
        if (paginasAlocadasNaRAM.size() == 1) {
            ponteiro = paginasAlocadasNaRAM.begin();
        }

        return 1; 
    }

    void rodarEmulador() { //loop de execução do simulador
        string entrada;
        string arg1;
        string arg2;
        string arg3;
        int PID;
        char command;
        list<Processo>::iterator itr;
        vector<int> novasPaginas;
        bool processoExiste;
        int IDMoldura;


        cout << "Emulador: Começando a execução do simulador!" << endl;
        while (true) {
            //pega o comando
            getline(cin, entrada);

            //comentario
            if (entrada[0] == '#')
                continue;

            //finaliza execucao
            if (entrada[0] == 'Q')
                break;
            
            vector<int> indexesOfSpaces;

            //pega o indice dos espacos
            for (int i = 0; i < (int)entrada.size(); i++) {
                if (entrada[i] == ' ')
                    indexesOfSpaces.push_back(i);
            }

            //pega cada pedaco do comando
            arg1 = entrada.substr(0, indexesOfSpaces[0]);
            arg2 = entrada.substr(indexesOfSpaces[0] + 1, indexesOfSpaces[1] - indexesOfSpaces[0] - 1);
            arg3 = entrada.substr(indexesOfSpaces[1] + 1, entrada.size() - (indexesOfSpaces[1] + 1));

            //cout << "1- |" << arg1 << "| 2- |" << arg2 << "| 3- |" << arg3 << "|" << endl;

            //PID EH O NUMERO DO PROCESSO
            PID = stoi(arg1.substr(1, arg1.size() - 1));

            //COMMAND EH A OPERACAO A SER FEITA
            command = arg2[0];

            //ARG3 PODE SER O ENDERECO OU TAMANHO, VOU MANTER COMO STRING

            //cout << "PID- |" << PID << "| COMANDO- |" << command << "| NUMERO- |" << arg3 << "|" << endl;

            //C, R, W, P , I
            switch (command) {
                //Criar processo
                case 'C':
                    //verifica se o ID nao esta sendo usado
                    processoExiste = false;
                    for (itr = processos.begin(); itr != processos.end(); itr++) {
                        if (itr->id == PID) {
                            cout << "Emulador: Ja existe um Processo com esse ID!" << endl;
                            processoExiste = true;
                            break;
                        }
                    }

                    if (processoExiste) { //se ja existe processo com esse ID, para
                        break;
                    }

                    //Aloca a memoria para o processo
                    novasPaginas = memSecundaria.criarPaginas(stoi(arg3));

                    //se conseguiu, cria o processo
                    if (novasPaginas[0] == -1) {
                        cout << "Emulador: Não existe memória em disco disponível para criar esse processo!" << endl;
                    }
                    else {
                        Processo* novo = new Processo(PID, stoi(arg3), novasPaginas);
                        processos.push_back(*novo);
                    }

                    break;

                //Matar processo (n sei se precisa fazer)
                case 'K':
                    //TODO
                    break;
                
                //Ler Memoria
                case 'R':
                    //procura o processo
                    processoExiste = false;
                    for(itr = processos.begin(); itr != processos.end(); itr++){
                        if (itr->id == PID) {
                            processoExiste = true;
                            break;
                        }
                    }
                    
                    if (!processoExiste) {
                        cout << "Emulador: Processo não Encontrado!" << endl;
                        break;
                    }

                    //pede pra ler na memoria
                    IDMoldura = itr->retornaIDMoldura(arg3);

                    if (IDMoldura == -1) { //Page Fault
                        //aloca a pagina na RAM
                        cout << "Emulador: Ocorreu uma PageFault!" << endl;
                        alocaPaginaRAM(PID, arg3);

                        //agora consegue ler
                        IDMoldura = itr->retornaIDMoldura(arg3);
                    }
                    else if (IDMoldura == -2) { //Endereço Inválido
                        cout << "Emulador: Endereço Inválido ou fora da área do programa!" << endl;
                        break;
                    }

                    //LE NA RAM (OU SO PRINTA KK)
                    cout << "Emulador: Processo " << PID << " leu no endereço " << arg3 << " que está na moldura " << IDMoldura << endl;

                    break;
                
                //Escrever Memoria
                case 'W':
                    //procura o processo
                    processoExiste = false;
                    for (itr = processos.begin(); itr != processos.end(); itr++) {
                        if (itr->id == PID) {
                            processoExiste = true;
                            break;
                        }
                    }

                    if (!processoExiste) {
                        cout << "Emulador: Processo não Encontrado!" << endl;
                        break;
                    }

                    //pede pra ler na memoria
                    IDMoldura = itr->retornaIDMoldura(arg3);

                    if (IDMoldura == -1) { //Page Fault
                        //aloca a pagina na RAM
                        cout << "Emulador: Ocorreu uma PageFault!" << endl;
                        alocaPaginaRAM(PID, arg3);

                        //agora ta na RAM
                        IDMoldura = itr->retornaIDMoldura(arg3);
                    }
                    else if (IDMoldura == -2) { //Endereço Inválido
                        cout << "Emulador: Endereço Inválido ou fora da área do programa!" << endl;
                        break;
                    }

                    //ESCREVE NA RAM (OU SO PRINTA KK)
                    cout << "Emulador: Processo " << PID << " escreveu no endereço " << arg3 << " que está na moldura " << IDMoldura << endl;

                    break;
                
                //Executar Instrução
                case 'P':
                    //procura o processo
                    processoExiste = false;
                    for (itr = processos.begin(); itr != processos.end(); itr++) {
                        if (itr->id == PID) {
                            processoExiste = true;
                            break;
                        }
                    }

                    if (!processoExiste) {
                        cout << "Emulador: Processo não Encontrado!" << endl;
                        break;
                    }

                    cout << "Emulador: O processo " << PID << "executou a instrucao "<< arg3 << endl;
                    break;
                
                //IO
                case 'I':
                    //procura o processo
                    processoExiste = false;
                    for (itr = processos.begin(); itr != processos.end(); itr++) {
                        if (itr->id == PID) {
                            processoExiste = true;
                            break;
                        }
                    }

                    if (!processoExiste) {
                        cout << "Emulador: Processo não Encontrado!" << endl;
                        break;
                    }

                    cout << "Emulador: O processo " << PID << "executou I/O no dispositivo " << arg3 << endl;
                    break;
                
                default:
                    cout << "Emulador: Entrada invalida!" << endl;
            }
        }
        cout << "Finalizando programa!" << endl;
    }
};

int main() {

    Emulador *teste = new Emulador();
    teste->rodarEmulador();

    return 0;
}

/*
QUESTION

quando criamos um procesos, quantas páginas reais dele alocamos?
podemos fazer "por demanda", só carrega quando precisar

*/


/*
Como funcionaria?

A classe emulador seria onde tudo acontece, ela recebe os comandos, faz os prints, e ela cuida de chamar
tudo que precisa, ela seria tipo o "main".

Ex1: comando P1 C 10 (criar processo de tam 10, pode ser 10 paginas)
    o emulador chamaria algum metodo da Memoria virtual pra ver se tem espaço sobrando (pq isso é tipo o disco, se
    n tiver espaço n dá pra criar mais programa kk). Se n tiver, n sei oq fazer, talvez avisar erro e é isso.
    Se tiver espaço, algum método da memoria virtual alocaria as paginas e retornaria os IDs delas, aí o Emulador
    criaria um Processo e passaria pra ele os IDs das paginas, o tamanho e o ID do processo em si, aí o construtor
    do processo se vira em organizar a tabela de páginas lá kk.
    E acho que isso resolve.

Ex2: comando P1 R 0
    o Emulador verifica se o processo 1 existe na lista de processos, se n existir, dá erro.
    Se existir, ele pede pro Processo acessar o endereço (que tá na tabela lá certin).
        Caso 1: endereço fora da região do programa -> printa erro e é isso
        Caso 2: endereço válido, e tá na memPrincipal -> acessa a página (por um metódo da memória sla) e printa os dados
        Caso 3: endereço válido, mas n tá na memPrincipal -> dá o page fault.

        Minha dúvida é, quem lida com o page fault? o Emulador? o Processo? a Memoria?
        (eu acho que o Emulador poderia lidar, simulando o SO, aí ele chamaria o algoritmo de substituição (que pode receber como argumento
        a tabela de páginas do processo, pra decidir qual delas pode sair da Principal, mas aí estaria limitado em só remover páginas do mesmo
        processo), podemos aceitar essa limitação, ou podemos fazer tipo tenta retirar dele, se n der tenta de outro processo, e vai tentando até conseguir liberar alguma moldura)
        Depois de liberar chama o método que deu o page fault, mas agr vai funcionar pq tá em RAM.

Algoritmos de Substituição:

    LRU:   Tira a menos utilizada recentemente, minha ideia é, cada vez que for ler ou escrever numa pagina, retira ela da lista (q lista? aaaaaaa) e insere no final, aí sempre que precisar
            só remover a primeira da lista (pq ela é a que tá a mais tempo sem ser utilizada).

    Relogio: Aqui precisa de um bit R e um ponteiro, mas a ideia da lista é semelhante da do LRU. (mas fazer ela circular aqui)

Vamos usar política de substituição global, páginas de qualquer processo podem ser retiradas.

Slide 7 da Aula20, processo criado é todo copiado para área de troca do disco, e suas páginas serão carregadas para RAM quando necessárias.

*/