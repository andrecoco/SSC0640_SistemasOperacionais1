#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "Memoria.h"
#include "Processo.h"
using namespace std;

//DEFINES
#define ALG_SUBSTITUICAO "LRU" //LRU ou RELOGIO
#define TAM_PAGINA 1024 //bytes, definimos que seja uma pot�ncia de 2
#define TAM_MEMORIA_SECUNDARIA 32 * TAM_PAGINA
#define TAM_MEMORIA_FISICA  16 * TAM_PAGINA

/*
    Ex: TAM_PAGINA 1024
        TAM_MEMORIA_SECUNDARIA 32 * TAM_PAGINA
        tamanho total = 32,768 (d� pra representar com 15 bits)
        como a pagina tem 1024 endere�os (representados com 10 bits), temos
                  endere�o (15 bits): 00101 1001011010
                  n_pagina (5 bits) : 00101
        endere�o na pagina(10 bits) :       1001011010

        Com isso s� pelo endere�o d� pra saber o n�mero da p�gina e o endere�o na p�gina :)
*/

/* OBS:
A gente pode definir os metodos dentro da classe, ou fora.
Ex:
class nomeClasse{
    void nomeMetodo(){
        //
    }
}

Ou d� pra fazer:
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


//FUNCOES AUXILIARES
int binary2Decimal(string binario) {
    int resultado = 0;
    for (int i = 0; i < binario.size(); i++) {
        resultado += (binario[i] - (int)'0') * pow(2, binario.size() - i - 1);
    }
    return resultado;
}


class Emulador { //seria o SO, todos os comandos passam por essa classe, ela distribui o resto
    bool useRelogio; //define se � rel�gio ou LRU
    list<Processo> processos;
    MemoriaPrincipal memPrincipal;
    MemoriaSecundaria memSecundaria;
    int numBitsEnderecoLogico;
    int numBitsIDPagina;

    //Coisas para algoritmo de substitui��o
    list<pair<int, int>> paginasAlocadasNaRAM; //par ID do processo e ID da pagina na memVirtual
    list<pair<int, int>>::iterator ponteiro; //pra utilizar no rel�gio, aponta para a p�gina mais antiga

public:
    Emulador() {
        if (ALG_SUBSTITUICAO == "RELOGIO") {
            useRelogio = true;
        }
        else {
            useRelogio = false;
        }

        ponteiro = paginasAlocadasNaRAM.end();

        //o endereco � dividido em dois peda�os, o primeiro � o ID da p�gina, o resto � o endere�o na p�gina
        numBitsEnderecoLogico = ceil(log2(TAM_MEMORIA_SECUNDARIA));
        numBitsIDPagina = numBitsEnderecoLogico - ceil(log2(TAM_PAGINA));
        //cout << numBitsEnderecoLogico << " " << numBitsIDPagina << endl;
        //cout << binary2Decimal("100111") << endl;
    }

    //Retira uma pagina da RAM a partir de um algoritmo de substituicao
    int algSubs() {
        int IDMoldura; //ID da moldura a ser liberada
        int IDPaginaVirtual; //ID da pagina virtual
        int PID; //ID do processo cuja pagina sera retirada da moldura
        list<Processo>::iterator itr;
        list<pair<int, int>>::iterator itrLista;

        if (useRelogio) { //Algoritmo do Relogio
            //TODO
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
                
                for (int i = 0; i < itr->tabelaPaginas.size(); i++) {
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
            //a pagina menos recentemente usada estar� no inicio da lista
            PID = paginasAlocadasNaRAM.front().first;
            IDPaginaVirtual = paginasAlocadasNaRAM.front().second;
        }
        
        //pega a tabela de paginas do processo e recupera o numero da moldura dessa pagina
        for (itr = processos.begin(); itr != processos.end(); itr++) {
            if (itr->id == PID) {
                break;
            }
        }

        for (int i = 0; i < itr->tabelaPaginas.size(); i++) {
            if (itr->tabelaPaginas[i].IDPagina == IDPaginaVirtual) {
                IDMoldura = itr->tabelaPaginas[i].numMoldura;
                    
                //Verifica se a pagina foi modificada enquanto em RAM
                if (itr->tabelaPaginas[i].M) {
                    //atualiza a copia na memoria secundaria
                    memSecundaria.atualizaPagina(IDPaginaVirtual, memPrincipal.retornaPagina(IDMoldura));
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
            }
        } 

        //Libera essa moldura da RAM
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
        int NumPaginaNoProcesso = binary2Decimal(endereco.substr(0, numBitsIDPagina - 1)); //o numero da pagina sao os numBitsIDPagina primeiros bits
        list<Processo>::iterator itr;
                                                                                           
        //pega a tabela de paginas do processo
        for (itr = processos.begin(); itr != processos.end(); itr++) {
            if (itr->id == PID) {
                //pega o ID da pagina virtual
                IDPaginaVirtual = itr->tabelaPaginas[NumPaginaNoProcesso].IDPagina;
                break;
            }
        }

        if (IDPaginaVirtual == -1) {
            cout << "Nao foi possivel encontrar a pagina!" << endl;
            return -1;
        }
        
        Pagina paginaVirtual = memSecundaria.retornaPaginaPeloID(IDPaginaVirtual);
        
        int retorno = memPrincipal.alocarPagina(paginaVirtual);
        
        if (retorno == -1) { //verifica se conseguiu alocar
            //se a RAM esta cheia, desaloca uma pagina
            algSubs();
            memPrincipal.alocarPagina(paginaVirtual);
        }
        
        //adiciona na lista de paginas na RAM
        paginasAlocadasNaRAM.push_back(pair<int, int>(itr->id, IDPaginaVirtual));

        //Se a RAM estivesse vazio, o ponteiro estar� errado
        if (ponteiro == paginasAlocadasNaRAM.end()) {
            ponteiro = paginasAlocadasNaRAM.begin();
        }

        return 1; 
    }

    void rodarEmulador() { //loop de execu��o do simulador
        string entrada;
        string arg1;
        string arg2;
        string arg3;
        int PID;
        char command;
        list<Processo>::iterator itr;
        bool processoExiste;
        int resposta;

        cout << "Come�ando a execu��o do simulador!" << endl;
        while (true) {
            //pega o comando
            getline(cin, entrada);

            if (entrada[0] == 'Q')
                break;

            vector<int> indexesOfSpaces;

            //pega o indice dos espacos
            for (int i = 0; i < entrada.size(); i++) {
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

            cout << "PID- |" << PID << "| COMANDO- |" << command << "| NUMERO- |" << arg3 << "|" << endl;

            //C, R, W, P , I
            switch (command) {
                //Criar processo
                case 'C':
                    //verifica se o ID nao esta sendo usado
                    for (itr = processos.begin(); itr != processos.end(); itr++) {
                        if (itr->id == PID) {
                            cout << "Ja existe um Processo com esse ID!" << endl;
                            break;
                        }
                    }
                    //coloca o processo no final da lista de processos
                    processos.push_back(Processo(PID, stoi(arg3)));

                    //checa se realmente conseguiu alocar o processo
                    if (processos.back().id == -1) {
                        cout << "N�o existe mem�ria em disco dispon�vel para criar esse processo!" << endl;
                        processos.pop_back();
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
                        cout << "Processo n�o Encontrado!" << endl;
                    }

                    //pede pra ler na memoria
                    resposta = itr->leMemoria(arg3);

                    if (resposta == 1) { //Page Fault
                        //aloca a pagina na RAM
                        alocaPaginaRAM(PID, arg3);

                        //agora consegue ler
                        itr->leMemoria(arg3);
                    }
                    else if (resposta == 2) { //Endere�o Inv�lido
                        cout << "Endere�o Inv�lido ou fora da �rea do programa!" << endl;
                    }
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
                        cout << "Processo n�o Encontrado!" << endl;
                    }

                    //pede pra ler na memoria
                    resposta = itr->escreveMemoria(arg3);

                    if (resposta == 1) { //Page Fault
                        //aloca a pagina na RAM
                        alocaPaginaRAM(PID, arg3);

                        //agora consegue ler
                        itr->escreveMemoria(arg3);
                    }
                    else if (resposta == 2) { //Endere�o Inv�lido
                        cout << "Endere�o Inv�lido ou fora da �rea do programa!" << endl;
                    }
                    break;
                
                //Executar Instru��o
                case 'P':
                    //TODO
                    break;
                
                //IO
                case 'I':
                    //TODO
                    break;
                
                default:
                    cout << "Entrada invalida!" << endl;
            }
        }
        cout << "Finalizando programa!" << endl;
    }
};

int main() {

    Emulador teste;
    teste.rodarEmulador();

    return 0;
}

/*
QUESTION

quando criamos um procesos, quantas p�ginas reais dele alocamos?
podemos fazer "por demanda", s� carrega quando precisar

*/


/*
Como funcionaria?

A classe emulador seria onde tudo acontece, ela recebe os comandos, faz os prints, e ela cuida de chamar
tudo que precisa, ela seria tipo o "main".

Ex1: comando P1 C 10 (criar processo de tam 10, pode ser 10 paginas)
    o emulador chamaria algum metodo da Memoria virtual pra ver se tem espa�o sobrando (pq isso � tipo o disco, se
    n tiver espa�o n d� pra criar mais programa kk). Se n tiver, n sei oq fazer, talvez avisar erro e � isso.
    Se tiver espa�o, algum m�todo da memoria virtual alocaria as paginas e retornaria os IDs delas, a� o Emulador
    criaria um Processo e passaria pra ele os IDs das paginas, o tamanho e o ID do processo em si, a� o construtor
    do processo se vira em organizar a tabela de p�ginas l� kk.
    E acho que isso resolve.

Ex2: comando P1 R 0
    o Emulador verifica se o processo 1 existe na lista de processos, se n existir, d� erro.
    Se existir, ele pede pro Processo acessar o endere�o (que t� na tabela l� certin).
        Caso 1: endere�o fora da regi�o do programa -> printa erro e � isso
        Caso 2: endere�o v�lido, e t� na memPrincipal -> acessa a p�gina (por um met�do da mem�ria sla) e printa os dados
        Caso 3: endere�o v�lido, mas n t� na memPrincipal -> d� o page fault.

        Minha d�vida �, quem lida com o page fault? o Emulador? o Processo? a Memoria?
        (eu acho que o Emulador poderia lidar, simulando o SO, a� ele chamaria o algoritmo de substitui��o (que pode receber como argumento
        a tabela de p�ginas do processo, pra decidir qual delas pode sair da Principal, mas a� estaria limitado em s� remover p�ginas do mesmo
        processo), podemos aceitar essa limita��o, ou podemos fazer tipo tenta retirar dele, se n der tenta de outro processo, e vai tentando at� conseguir liberar alguma moldura)
        Depois de liberar chama o m�todo que deu o page fault, mas agr vai funcionar pq t� em RAM.

Algoritmos de Substitui��o:

    LRU:   Tira a menos utilizada recentemente, minha ideia �, cada vez que for ler ou escrever numa pagina, retira ela da lista (q lista? aaaaaaa) e insere no final, a� sempre que precisar
            s� remover a primeira da lista (pq ela � a que t� a mais tempo sem ser utilizada).

    Relogio: Aqui precisa de um bit R e um ponteiro, mas a ideia da lista � semelhante da do LRU. (mas fazer ela circular aqui)

Vamos usar pol�tica de substitui��o global, p�ginas de qualquer processo podem ser retiradas.

Slide 7 da Aula20, processo criado � todo copiado para �rea de troca do disco, e suas p�ginas ser�o carregadas para RAM quando necess�rias.

*/