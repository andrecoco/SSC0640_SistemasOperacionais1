//AS CLASSES
/*Moldura e Pagina eh onde a memoria realmente ta alocada*/
class Pagina {
public: //tudo antes disso eh private por default
    vector<char> dados; //o conte�do de endere�o vai ser representado por 1 byte
    string endereco;
    int id;

    Pagina(int id) {
        this->id = id;
    }

    Pagina(string endereco) {
        this->endereco = endereco;
    }

    Pagina(string endereco, int id) {     // Construtor
        this->endereco = endereco;
        this->id = id;
    }

    Pagina(string endereco, char dados) {
        this->endereco = endereco;
        this->dados = dados;
    }
};

class Moldura {
public: //tudo antes disso eh private por default
    Pagina pagina;

    Moldura() {     // Construtor
      //TODO
    }
};

class MemoriaSecundaria { //isso seria representando o disco, armazena as p�ginas virtuais
public: //tudo antes disso eh private por default
    vector<bool> paginasOcupadas; //guarda se cada pagina ta utilizada ou n
    vector<Pagina> paginas; //guardam as paginas de todos os processos

    MemoriaSecundaria() {     // Construtor
      //TODO
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
        static int newPageId = 1;
        vector<int> paginasCriadas;

        for (int i = 0; i < n; i++) {
            Pagina *page = new Pagina(newPageId);
            if (page == NULL) { //se erro na criacao da pagina, id == -1
                paginasCriadas.push_back(-1);
                continue;
            } else {
                page->id = newPageId;
                newPageId++;
            }

            paginasCriadas.push_back(page->id);
        }
    }

    //atualiza a pagina por essa nova pagina passada (tipo copiar os dados)
    int atualizaPagina(int IDPaginaVirtual, Pagina novaPagina) {
        //TODO
        return 1;
    }

    //exclui a pagina com esse ID
    int deletaPagina(int IDPaginaVirtual) {
        //TODO
    }

    //retorna uma copia da pagina com esse ID
    Pagina retornaPaginaPeloID(int IDPaginaVirtual) {
       Pagina *copia;
       bool encontrou = false;

       //procura a pagina desejada no vetor de paginas
       for (int i = 0; i < paginas.size(); i++) {
            if (paginas[i].id == IDPaginaVirtual) {
                copia = &(paginas[i]);
                encontrou = true;
                break;
            }
       }

       if (!encontrou) {
            return NULL;
       }
        
        return copia;
    }

    

};

class MemoriaPrincipal {

    public: //tudo antes disso eh private por default
    vector<bool> moldurasOcupadas; //guarda se cada moldura ta ocupada ou n
    vector<Moldura> molduras; //aqui seria tipo a memoria msm, "bruta"

    MemoriaPrincipal() {     // Construtor
      //TODO
    }

    //retorna se a memoria esta cheia
    bool isFull() {
        for (int i = 0; i < moldurasOcupadas.size(); i++)
            if (!moldurasOcupadas[i]) //se tem alguma moldura livre
                return false;

        return true;
    }

    //retornar uma copia da pagina nessa moldura
    Pagina retornaPagina(int IDMoldura) {
        //TODO
        Pagina teste;
        return teste;
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