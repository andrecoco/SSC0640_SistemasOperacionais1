#ifndef PROCESSO_H
#define PROCESSO_H
#include <vector>
#include <string>
using namespace std;


class EntradaTabelaPagina {
    /*
        - Num da moldura da pag
        - Bit presente/ausente
        - Bits Mod e/ou Referenciada (pra usar nos algs de subs)
    */
    public: //tudo antes disso eh private por default
    int IDPagina; //id da pagina virtual
    bool presente; //se est� em alguma moldura ou n�o
    bool M; //se foi modificado enquanto na RAM (mem�ria principal)
    bool R; //usado no algoritmo do relogio
    int numMoldura; //id da moldura onde a p�gina est�


    EntradaTabelaPagina() {     // Construtor
      //TODO
    }
};

class Processo {

    public: //tudo antes disso eh private por default
    //cada processo tem sua tabela de pagina
    int id; //numero do processo
    int tamanho; //tamanho do processo
    vector<EntradaTabelaPagina> tabelaPaginas;

    Processo(int pid, int tam) {     // Construtor
        id = pid;
        tamanho = tam;

        //TEM QUE INICIALIZAR A TABELA DE PAGINAS TAMBEM
        //SE NAO CONSEGUIR CRIAR O PROCESSO, COLOCA O ID DELE COMO -1, QUE O EMULADOR ENTENDE COMO ERRO
        //TODO

    }

    //tenta ler algo nesse endere�o
    int leMemoria(string endereco) {
        /*
        Tabela de Retornos
        0 - Conseguiu Ler
        1 - PageFault (pagina n est� na RAM)
        2 - Endereco Invalido e/ou Fora da area do processo
        */

        //TODO

        return 0; //conseguiu ler
    }

    //tenta escrever algo nesse endere�o
    int escreveMemoria(string endereco) {
        /*
        Tabela de Retornos
        0 - Conseguiu Escrever
        1 - PageFault (pagina n est� na RAM)
        2 - Endereco Invalido e/ou Fora da area do processo
        */

        //TODO

        return 0; //conseguiu escrever
    }
};

#endif