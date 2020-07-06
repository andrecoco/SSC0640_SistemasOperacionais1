#ifndef AUXILIAR_H
#define AUXILIAR_H
#include <string>
#include <math.h>
using namespace std;

//FUNCOES AUXILIARES
int binary2Decimal(string binario) {
    int resultado = 0;
    for (int i = 0; i < (int)binario.size(); i++) {
        resultado += (binario[i] - '0') * (int)pow(2, binario.size() - i - 1);
    }
    return resultado;
}

#endif