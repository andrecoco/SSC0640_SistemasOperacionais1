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

        cout << "############################## EMULADOR ############################" << endl;
        cout << "O endereco logico possui " << numBitsEnderecoLogico << "bits" << endl;
        cout << "Os primeiros " << numBitsIDPagina << "bits representam o numDaPagina" << endl;
        if (useRelogio) {
            cout << "O algoritmo de substituicao e o Relogio" << endl;
        }
        else {
            cout << "O algoritmo de substituicao e o LRU" << endl;
        }
        cout << "####################################################################" << endl << endl;

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
                            
                            if(ponteiro == paginasAlocadasNaRAM.begin()){
                                ponteiro = paginasAlocadasNaRAM.end();
                                ponteiro--;
                            }
                            else {
                                ponteiro--;
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

                if (ponteiro == paginasAlocadasNaRAM.begin()) {
                    ponteiro = paginasAlocadasNaRAM.end();
                    ponteiro--;
                }
                else {
                    ponteiro--;
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
        if (!useRelogio) { //LRU
            paginasAlocadasNaRAM.push_back(pair<int, int>(itr->id, IDPaginaVirtual));
        }
        else { //Relogio
            //insere a pagina na posicao anterior a do ponteiro
            if (paginasAlocadasNaRAM.size() == 1) {//Se a RAM estivesse vazia, o ponteiro n estará setado
                paginasAlocadasNaRAM.push_back(pair<int, int>(itr->id, IDPaginaVirtual));
                ponteiro = paginasAlocadasNaRAM.begin();
            }
            else {
                list<pair<int, int>>::iterator posicao = ponteiro;
                posicao--;
                paginasAlocadasNaRAM.insert(posicao, pair<int, int>(itr->id, IDPaginaVirtual));
            }
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
        list<pair<int, int>>::iterator itrLista;
        vector<int> novasPaginas;
        bool processoExiste;
        int IDMoldura;
        int IDPagina;
        int numPagina;


        cout << "Emulador: Comecando a execucao do simulador!" << endl;
        while (true) {
            //pega o comando
            getline(cin, entrada);
               
            //comentario
            if (entrada[0] == '#')
                continue;

            cout << "Entrada: " << entrada << endl;

            //finaliza execucao
            if (entrada[0] == 'Q')
                break;
            
            vector<int> indexesOfSpaces;

            //pega o indice dos espacos
            for (int i = 0; i < (int)entrada.size(); i++) {
                if (entrada[i] == ' ')
                    indexesOfSpaces.push_back(i);
            }

            if (indexesOfSpaces.size() < 2) {
                cout << "Emulador: Entrada invalida!" << endl;
                continue;
            }

            //pega cada pedaco do comando
            arg1 = entrada.substr(0, indexesOfSpaces[0]);
            arg2 = entrada.substr(indexesOfSpaces[0] + 1, indexesOfSpaces[1] - indexesOfSpaces[0] - 1);
            arg3 = entrada.substr(indexesOfSpaces[1] + 1, entrada.size() - (indexesOfSpaces[1] + 1));

            //PID EH O NUMERO DO PROCESSO
            PID = stoi(arg1.substr(1, arg1.size() - 1));

            //COMMAND EH A OPERACAO A SER FEITA
            command = arg2[0];

            //ARG3 PODE SER O ENDERECO OU TAMANHO, VOU MANTER COMO STRING

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
                        cout << "Emulador: Nao existe memoria em disco disponivel para criar esse processo!" << endl;
                    }
                    else {
                        cout << "Emulador: Processo " << PID << " criado com sucesso!" << endl;
                        Processo* novo = new Processo(PID, stoi(arg3), novasPaginas);
                        processos.push_back(*novo);
                    }

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
                        cout << "Emulador: Processo nao Encontrado!" << endl;
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
                        cout << "Emulador: Endereco Invalido ou fora da area do programa!" << endl;
                        break;
                    }

                    //LE NA RAM
                    //marca que a pagina foi referenciada
                    numPagina = binary2Decimal(arg3.substr(0, numBitsIDPagina));
                    itr->tabelaPaginas[numPagina].R = true;

                    //se for LRU, coloca ela no final
                    if (!useRelogio) {
                        IDPagina = itr->tabelaPaginas[numPagina].IDPagina;
                        for (itrLista = paginasAlocadasNaRAM.begin(); itrLista != paginasAlocadasNaRAM.end(); itrLista++) {
                            if (itrLista->second == IDPagina) {
                                paginasAlocadasNaRAM.push_back(pair<int,int>(itrLista->first, itrLista->second));
                                paginasAlocadasNaRAM.erase(itrLista);
                                break;
                            }
                        }
                    }

                    //printa
                    cout << "Emulador: Processo " << PID << " leu no endereco " << arg3 << " que esta na moldura " << IDMoldura << endl;

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
                        cout << "Emulador: Processo nao Encontrado!" << endl;
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
                        cout << "Emulador: Endereço Invalido ou fora da area do programa!" << endl;
                        break;
                    }

                    //ESCREVE NA RAM
                    //marca que a pagina foi referenciada e modificada
                    numPagina = binary2Decimal(arg3.substr(0, numBitsIDPagina));
                    IDPagina = itr->tabelaPaginas[numPagina].IDPagina;
                    itr->tabelaPaginas[numPagina].R = true;
                    itr->tabelaPaginas[numPagina].M = true;

                    //printa
                    cout << "Emulador: Processo " << PID << " escreveu no endereco " << arg3 << " que esta na moldura " << IDMoldura << endl;

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
                        cout << "Emulador: Processo nao Encontrado!" << endl;
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
                        cout << "Emulador: Processo nao Encontrado!" << endl;
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
