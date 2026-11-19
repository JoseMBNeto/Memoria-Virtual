#include <stdio.h>
#include <stdlib.h>

#include "entrada.h"
#include "memoria.h"
#include "tlb.h"

int main(int argc, char *argv[]){
    const char *arquivo_enderecos;
    int alg_pagina;
    int alg_tlb;
    int *enderecos;
    int quantidade;
    int i;

    if (!ler_argumentos(argc, argv, &arquivo_enderecos, &alg_pagina, &alg_tlb)){
        return EXIT_FAILURE;
    }

    enderecos = ler_enderecos(arquivo_enderecos, &quantidade);

    if (enderecos == NULL){
        return EXIT_FAILURE;
    }

    printf("pagina: %s | TLB: %s | enderecos: %d\n", alg_pagina == ALG_FIFO ? "fifo" : "lru", alg_tlb == ALG_FIFO ? "fifo" : "lru", quantidade);
    
    for (i = 0; i < quantidade && i < 3; i++){
        printf("endereco %d -> pagina %d, offset %d\n", enderecos[i], numero_pagina(enderecos[i]), numero_offset(enderecos[i]));
    }
    
    printf("ultimo: endereco %d -> pagina %d, offset %d\n", enderecos[quantidade - 1], numero_pagina(enderecos[quantidade - 1]), numero_offset(enderecos[quantidade - 1]));

    free(enderecos);
    return EXIT_SUCCESS;
}