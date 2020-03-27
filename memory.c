#include <stdlib.h>
#include <stdio.h>

/*
Este codigo simples foi feito para testar 3 primitivas de chamada de sistema do Linux 
referentes a categoria de gerenciamento de memoria. Sao elas:
- brk()
- mmap()
- mprotect()
*/

void multiplicacao(int* a, int* b, int* resultado){
    *resultado = (*a)*(*b);
}

int main(){
    //Aloca espaço para 3 inteiros na memoria
    int* resultado = malloc(sizeof(int));
    int* a = malloc(sizeof(int));
    int* b = malloc(sizeof(int));
    
    //Define valores
    *a = 5;
    *b = 5;

    //Multiplica os valores
    multiplicacao(a,b,resultado);
    
    //Imprime o resultado
    printf("%d\n", *resultado);
    
    free(resultado);
    free(a);
    free(b);
    return 0;
}