#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

//Esse define escolhe qual 'teste' vai ser realizado
// 0 - Deixa todas as variaveis com permissao para leitura e escrita
// 1 - Restringe permissoes da variavel 'a'
// 2 - Restringe permissoes da variavel 'resultado'
#define DEFINEPROTECAO 0

/*
Este codigo foi feito para testar 3 primitivas de chamada de sistema do Linux 
referentes a categoria de gerenciamento de memoria. Sao elas:
- mmap()
- mprotect()
- munmap()
*/

void multiplicacao(int* a, int* b, int* resultado){
    *resultado = (*a)*(*b);
}

int main(){
    //Aloca espaço para 3 inteiros na memoria
    int *resultado = mmap(0, sizeof(int), PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    int *a = mmap(0, sizeof(int), PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    int *b = mmap(0, sizeof(int), PROT_WRITE | PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    //Define valores para os inteiros
    *a = 5;
    *b = 7;

    //Define a protecao da regiao de memoria
    switch (DEFINEPROTECAO){
        case 1:
            mprotect(a, 4, PROT_NONE); //Remove permissoes de acesso/escrita a variavel 'a'
        break;
        
        case 2:
            mprotect(resultado, 4, PROT_NONE); //Remove permisssoes de acesso/escrita a variavel 'resultado'
        break;
    }

    //Multiplica os valores
    printf("Vamos multiplicar %d * %d\n", *a, *b);
    multiplicacao(a,b,resultado);
    
    //Imprime o resultado
    printf("Resultado: %d\n", *resultado);
    
    //Disaloca a memoria alocada com o mmap
    munmap(resultado, 4);
    munmap(a, 4);
    munmap(b, 4);
    return 0;
}