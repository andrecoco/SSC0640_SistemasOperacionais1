#include <stdlib.h>
#include <stdio.h>
/*
Programa feito para exemplificar um processo IO-bound.
Ele pede que o usuario digite o nome de um arquivo, o programa buscara esse
arquivo no diretorio dele, e caso encontre, vai printar o conteudo dele na tela.
*/

int main(){
    char nomeArquivo[256];
    char linhaLida[256];
    FILE* arquivo;
    printf("Por favor digite o nome do arquivo a ser lido:\n");
    scanf("%s", nomeArquivo);
    
    arquivo = fopen( nomeArquivo, "r");
    if(arquivo == NULL){
        printf("Arquivo n existente!\n");
        return 1;
    }

    printf("###############\n");
    while(fgets(linhaLida, 255 ,arquivo) != NULL){
        printf("%s", linhaLida);
    }

    return 0;
}