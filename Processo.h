#ifndef PROCESSO_H
#define PROCESSO_H
#include <vector>
#include <string>
#include "Auxiliar.h"
#include "Memoria.h"
using namespace std;


class EntradaTabelaPagina {
    /*
        - Num da moldura da pag
        - Bit presente/ausente
        - Bits Mod e/ou Referenciada (pra usar nos algs de subs)
    */
    public: //tudo antes disso eh private por default
    int IDPagina; //id da pagina virtual
    bool presente; //se está em alguma moldura ou não
    bool M; //se foi modificado enquanto na RAM (memória principal)
    bool R; //usado no algoritmo do relogio
    int numMoldura; //id da moldura onde a página está


    EntradaTabelaPagina(int id) {     // Construtor
        IDPagina = id;
        presente = false;
        M = false;
        R = false;
        numMoldura = -1;
    }
};

class Processo {

    public: //tudo antes disso eh private por default
    //cada processo tem sua tabela de pagina
    int id; //numero do processo
    int tamanho; //tamanho do processo (em n paginas)
    vector<EntradaTabelaPagina> tabelaPaginas;

    Processo(int pid, int tam, vector<int> IDPaginasVirtuais) {     // Construtor
        id = pid;
        tamanho = tam;
        
        for (int i = 0; i < (int)IDPaginasVirtuais.size(); i++) {
            EntradaTabelaPagina* nova = new EntradaTabelaPagina(IDPaginasVirtuais[i]);
            tabelaPaginas.push_back(*nova);
        }

    }

    int retornaIDPagina(int numPagina) {
        /*
        Tabela de Retornos
        -1 - Pagina n existe
        Inteiro positivo - ID Pagina virtual
        */

        if (numPagina > (int)tabelaPaginas.size()) {
            return -1;
        }

        return tabelaPaginas[numPagina].IDPagina;
    }

    //retorna o id da moldura onde este endereço esta
    int retornaIDMoldura(string endereco) {
        /*
        Tabela de Retornos
        inteiro positivo - moldura esta na RAM
        -1 - PageFault (pagina n está na RAM)
        -2 - Endereco Invalido e/ou Fora da area do processo
        */
        int numBitsEnderecoNaPagina = (int)ceil(log2(TAM_PAGINA));
        int numBitsIDPagina = (int)ceil(log2(TAM_MEMORIA_SECUNDARIA)) - (int)ceil(log2(TAM_PAGINA));
        
        //verifica se o endereco possui o numero de bits correto
        if (endereco.size() != (numBitsIDPagina + numBitsEnderecoNaPagina)) {
            return -2; //endereco invalido
        }

        //pega o num da pagina e o 'deslocamento'
        string SnumPagina = endereco.substr(0 , numBitsIDPagina);
        string SenderecoNaPagina = endereco.substr(numBitsIDPagina, numBitsEnderecoNaPagina);
        int numPagina = binary2Decimal(SnumPagina);
        int enderecoNaPagina = binary2Decimal(SenderecoNaPagina);

        //cout << "DEBUGG: " << numPagina << endl;

        //verifica se o processo possui essa pagina
        if ((int)tabelaPaginas.size() <= numPagina) {
            return -2; //endereco invalido
        }

        //verifica se a pagina esta na RAM
        if (!tabelaPaginas[numPagina].presente) {
            return -1; //pagina n esta na RAM
        }

        return tabelaPaginas[numPagina].numMoldura;
    }

};

#endif