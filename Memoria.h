#ifndef MEMORIA_H
#define MEMORIA_H
#include <vector>
#include <iostream>
#include <string>
#include "Auxiliar.h"
using namespace std;

//defines
#define TAM_PAGINA 1024 //bytes, definimos que seja uma potência de 2
#define TAM_MEMORIA_SECUNDARIA 6 * TAM_PAGINA
#define TAM_MEMORIA_FISICA  3 * TAM_PAGINA

//AS CLASSES
/*Moldura e Pagina eh onde a memoria realmente ta alocada*/
class Pagina {
public: //tudo antes disso eh private por default
    vector<char> dados; //o conteúdo de endereco vai ser representado por 1 byte
    int id;

    Pagina() {
        vector<char>* novo = new vector<char>(TAM_PAGINA);
        this->dados = *novo;
        this->id = -1;
    }

    //Construtores
    Pagina(int id) { 
        vector<char>* novo = new vector<char>(TAM_PAGINA);
        this->dados = *novo;
        this->id = id;
    }

    Pagina(int id, vector<char> dados) {
    	this->id = id;
    	this->dados = dados;
    }
};

class Moldura {
public: //tudo antes disso eh private por default
    Pagina pagina;
    int id;

    Moldura() { // Construtor default
        pagina = *(new Pagina());
        this->id = -1;
    }

    //Demais construtores
    Moldura(int id) {
    	this->id = id;
    }

    Moldura(int id, Pagina pagina) {
    	this->id = id;
    	this->pagina = pagina;
    }
};

class MemoriaSecundaria { //isso seria representando o disco, armazena as páginas virtuais
public: //tudo antes disso eh private por default
    vector<bool> paginasOcupadas; //guarda se cada pagina ta utilizada ou n
    vector<Pagina> paginas; //guardam as paginas de todos os processos
    
    MemoriaSecundaria() {     // Construtor default
        this->paginas = *(new vector<Pagina>(TAM_MEMORIA_SECUNDARIA / TAM_PAGINA));
        for (int i = 0; i < (int)paginas.size(); i++) { //seta o id das paginas (o construtor padrao coloca -1 em tudo)
            paginas[i].id = i;
        }

        paginasOcupadas = *(new vector<bool>(paginas.size(), false)); //inicialmente estao todas vazias

    }

    //Demais construtores
    MemoriaSecundaria(vector<Pagina> paginas) {
    	this->paginas = paginas;
    }

    MemoriaSecundaria(vector<bool> paginasOcupadas, vector<Pagina> paginas) {
    	this->paginasOcupadas = paginasOcupadas;
    	this->paginas = paginas;
    }

    //retorna se a memoria esta cheia
    bool isFull() {
        for (int i = 0; i < (int)paginasOcupadas.size(); i++)
            if (!paginasOcupadas[i]) //se tem alguma pagina livre
                return false;

        return true;
    }

    //retorna se existem pelo menos n paginas virtuais livres
    bool canFit(int n) {
        int contador = 0;

        for (int i = 0; i < (int)paginasOcupadas.size() && contador <= n; i++)
            if (!paginasOcupadas[i]) //se essa pagina estiver livre
                contador++;

        return contador >= n;
    }

    //aloca n paginas e retorna o ID delas (ou {-1} se n conseguir)
    vector<int> criarPaginas(int n) {
    	vector<int> paginasCriadas;

        //verifica se cabe
        if(canFit(n)){
            int contador = 0;
            for (int i = 0; contador < n; i++) {
                if (!paginasOcupadas[i]) { //se estiver livre
                    paginasOcupadas[i] = true;
                    paginasCriadas.push_back(i);
                    contador++;
                }
            }
        }
        else {
            return { -1 };
        }
        /*
    	if (paginas.empty()) {
    		static int newPageId = 0;

	        for (int i = 0; i < n; i++) {
	            Pagina *page = new Pagina(newPageId);
	            if (page == NULL) { //se erro na criacao da pagina, id == -1
	                paginasCriadas.push_back(-1);
	                continue;
	            } else {
	                newPageId++;
	            }

	            paginasCriadas.push_back(page->id);
	        }
    	} else {
    		int newID = paginas.back().id + 1;
    		for (int i = 0; i < n; i++) {
    			Pagina *page = new Pagina(newID);
	            if (page == NULL) { //se erro na criacao da pagina, id == -1
	                paginasCriadas.push_back(-1);
	                continue;
	            } else {
	                newID++;
	            }

	            paginasCriadas.push_back(page->id);
    		}
    	}*/

    	return paginasCriadas; 
    }

    //atualiza a pagina por essa nova pagina passada
    //Se atualizado com sucesso, retorna 0
    //Senão, retorna 1
    int atualizaPagina(int IDPaginaVirtual, Pagina novaPagina, bool modifBit) {
    	int index;
        Pagina *pagAntiga = buscaPagina(IDPaginaVirtual, &index);
        int atualizou = 1; //flag de verificacao

        if (modifBit) {
        	*pagAntiga = novaPagina;
        	atualizou = 0;
        }

        return atualizou;
    }

    //exclui a pagina com esse ID
    //Se deletado com sucesso, retorna 0
    //Senão, retorna 1
    int deletaPagina(int IDPaginaVirtual) {
    	int index = -1; //indice da pagina desejada no vector de paginas
    	int deletou = 1; //flag de verificacao

    	Pagina *pag = buscaPagina(IDPaginaVirtual, &index);
    	if (pag->id != -1 && index != -1) {
    		paginas.erase(paginas.begin() + index);
    		deletou = 0;
    	}

    	return deletou;
    }

     //verifica se existe o endereco desejado na pagina virtual (se esta entre 0 e TAM_PAGINA bytes)
    bool enderecoExiste(string endereco) {
    	if (stoi(endereco) >= 0 && stoi(endereco) < TAM_PAGINA) {
    		return true;
    	} else {
    		return false;
    	}
    }

    //retorna uma copia da pagina com esse ID
    //se pagina com ID inexistente retorna uma pagina com ID = -1
    Pagina retornaPaginaPeloID(int IDPaginaVirtual) {
    	Pagina copia, *naoEncontrou = new Pagina(-1);

       for (int i = 0; i < (int)paginas.size(); i++) {
            if (paginas[i].id == IDPaginaVirtual) {
                copia = paginas[i];
                return copia;
            }
       }

       return (*naoEncontrou); //Pagina com id = -1
    }

    //diferente da funcao acima, ela retorna a propria pagina, e nao uma copia
    Pagina* buscaPagina(int IDPaginaVirtual, int* index) {
    	Pagina *naoEncontrou = new Pagina(-1); //instancia uma pagina com id = -1, representando uma pagina inexistente

    	for (int i = 0; i < (int)paginas.size(); i++) {
    		if (paginas[i].id == IDPaginaVirtual) {
    			*index = i;
            	return &(paginas[i]);
        	}
    	}

    	return naoEncontrou;
    }

};

class MemoriaPrincipal {

    public: //tudo antes disso eh private por default
    vector<bool> moldurasOcupadas; //guarda se cada moldura ta ocupada ou n
    vector<Moldura> molduras; //aqui seria tipo a memoria msm, "bruta"

    MemoriaPrincipal() {     // Construtor default
        this->molduras = *(new vector<Moldura>(TAM_MEMORIA_FISICA / TAM_PAGINA));
        for (int i = 0; i < (int)molduras.size(); i++) {
            molduras[i].id = i; //seta os ids, que inicialmente eram -1
        }

    	this->moldurasOcupadas = *(new vector<bool>(molduras.size(), false)); //na criacao, todas as molduras estao disponiveis
    }

    //Demais construtores
    MemoriaPrincipal(vector<Moldura> molduras) {
    	this->molduras = molduras;
    }

    MemoriaPrincipal(vector<bool> moldurasOcupadas, vector<Moldura> molduras) {
    	this->moldurasOcupadas = moldurasOcupadas;
    	this->molduras = molduras;
    }

    //retorna se a memoria esta cheia
    bool isFull() {
        for (int i = 0; i < (int)moldurasOcupadas.size(); i++)
            if (!moldurasOcupadas[i]) //se tem alguma moldura livre
                return false;

        return true;
    }

    //busca a moldura pelo seu ID e retorna o indice da moldura no vector molduras
    int buscaMoldura(int IDMoldura) {
    	for (int i = 0; i < (int)molduras.size(); i++) {
    		if (IDMoldura == molduras[i].id) {
    			return i;
    		}
    	}
    	return -1; //se nao achou
    }

    //retorna uma copia da pagina nessa moldura
    //se pagina inexistente, retorna uma pagina com ID = -1
    Pagina retornaPagina(int IDMoldura) {
    	Pagina *pag = new Pagina(-1), copia;
        int indexMoldura = buscaMoldura(IDMoldura);

        if (indexMoldura >= 0) {
        	copia = molduras[indexMoldura].pagina;
        } else {
        	copia = (*pag);
        }

        return copia;
    }

    //retorna o indice da proxima moldura disponivel na memoria
    int nextFreeMoldura() {
    	for (int i = 0; i < (int)moldurasOcupadas.size(); i++) {
    		if (!moldurasOcupadas[i]) {
    			return i;
    		}
    	}

    	return -1; //nao ha molduras livres
    }

    //aloca a pagina na memoria principal, retorna o ID da pagina alocada (ou -1 caso a RAM esteja cheia)
    int alocarPagina(Pagina pagina) {
        bool memCheia = isFull();
        int nextMolduraIndex;
        if (!memCheia) {
        	nextMolduraIndex = nextFreeMoldura();
        	molduras[nextMolduraIndex].pagina = pagina;
            moldurasOcupadas[nextMolduraIndex] = true;
        } else {
        	return -1; //erro: memoria cheia. pagina NAO alocada.
        }

        return molduras[nextMolduraIndex].id; //sucesso. pagina alocada.
    }

    //desaloca essa pagina da RAM (recebe o id da moldura)
    //pagina com id = -2 --> pagina desalocada
    int desalocarPagina(int IDMoldura) {
        /*for (int i = 0; i < molduras.size(); i++) {
        	if (molduras[i].pagina.id == idPagina) {
        		int idMoldura = molduras[i].id;
        		molduras[i].pagina.id = -2;
        		moldurasOcupadas[i] = false;
        		return idMoldura;
        	}
        }*/
        
        moldurasOcupadas[IDMoldura] = false;

        return -1; //moldura nao encontrada
    }
};

#endif