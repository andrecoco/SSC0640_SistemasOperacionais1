#ifndef MEMORIA_H
#define MEMORIA_H
#include <vector>
#include <iostream>
using namespace std;


//AS CLASSES
/*Moldura e Pagina eh onde a memoria realmente ta alocada*/
class Pagina {
public: //tudo antes disso eh private por default
    vector<char> dados; //o conteúdo de endereco vai ser representado por 1 byte
    int id;

    Pagina() { //Construtor default

    }

    //Demais Construtores
    Pagina(int id) { 
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

    Moldura() {     // Construtor default
      
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
        for (int i = 0; i < paginasOcupadas.size(); i++)
            if (!paginasOcupadas[i]) //se tem alguma pagina livre
                return false;

        return true;
    }

    //retorna se existem pelo menos n paginas virtuais livres
    bool canFit(int n) {
        int contador = 0;

        for (int i = 0; i < paginasOcupadas.size() && contador <= n; i++)
            if (!paginasOcupadas[i]) //se essa pagina estiver livre
                contador++;

        return contador >= n;
    }

    //aloca n paginas e retorna o ID delas
    vector<int> criarPaginas(int n) {
    	vector<int> paginasCriadas;

    	if (paginas.empty()) {
    		cout << "paginas.empty" << endl;
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
	            cout << "fim empty" << endl;
	        }
    	} else {
    		cout << "paginas nao empty" << endl;
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
	            cout << "fim nao empty" << endl;
    		}
    	}

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

     //verifica se existe o endereco desejado na pagina virtual (se esta entre 0 e 1023 bytes)
    bool enderecoExiste(string endereco) {
    	if (stoi(endereco) >= 0 && stoi(endereco) <= 1023) {
    		return true;
    	} else {
    		return false;
    	}
    }

    //retorna uma copia da pagina com esse ID
    //se pagina com ID inexistente retorna NULL
    Pagina retornaPaginaPeloID(int IDPaginaVirtual) {
    	Pagina copia, *naoEncontrou = new Pagina(-1);

       for (int i = 0; i < paginas.size(); i++) {
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

    	for (int i = 0; i < paginas.size(); i++) {
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

    	this->moldurasOcupadas = vector<bool> (16, false); //na criacao, todas as molduras estao disponiveis
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
        for (int i = 0; i < moldurasOcupadas.size(); i++)
            if (!moldurasOcupadas[i]) //se tem alguma moldura livre
                return false;

        return true;
    }

    //busca a moldura pelo seu ID e retorna o indice da moldura no vector molduras
    int buscaMoldura(int IDMoldura) {
    	for (int i = 0; i < molduras.size(); i++) {
    		if (IDMoldura == molduras[i].id) {
    			return i;
    		}
    	}
    }

    //retornar uma copia da pagina nessa moldura
    Pagina retornaPagina(int IDMoldura) {
        Pagina pag;
        return pag;
    }

    //aloca a pagina na memoria principal, retorna o ID da pagina alocada (ou -1 caso a RAM esteja cheia)
    int alocarPagina(Pagina pagina) {
        //TODO
        return 0;
    }

    //desaloca essa pagina da RAM (recebe o id da moldura)
    int desalocarPagina(int id) {
        //TODO
        return 0;
    }
};

#endif