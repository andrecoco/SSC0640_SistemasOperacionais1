#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX 125000

/*
Programa feito para exemplificar um processo CPU-bound.
Ele calcula todos os numeros primos menores que MAX.
*/

int main()
{
    int num;
    int div;
    int marcador;
    long unsigned int contador_passos = 0;
    long unsigned int contador_primos = 0;
    
    //Vamos ignorar os "semi-primos" (2 e 3)
    contador_primos = 2; 
    
    //Testa os numeros entre 5 e MAX
    for(num = 5; num < MAX; num +=2)
    {
        marcador = 0;
        for(div = 2; div < (num/2); div += 1)
        {
            contador_passos++;
            if(num%div == 0)
            {
                marcador = 1;
            }
        }
        if(marcador == 0)
        {
            contador_primos++;
        }
    }
    printf ("Gastei %ld passsos para encontrar %ld numeros primos!\n", contador_passos, contador_primos);
    return 0;
}
