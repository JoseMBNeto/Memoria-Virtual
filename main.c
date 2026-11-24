#include <stdio.h>
#include <stdlib.h>
#include "entrada.h"
#include "memoria.h"
#include "tlb.h"
#define ARQUIVO_BACKING_STORE "BACKING_STORE.bin"
static Memoria memoria;

int main(int argc, char *argv[]){
    const char *arquivo_enderecos;
    int alg_pagina;
    int alg_tlb;
    int *enderecos;
    int quantidade;
    Tlb tlb;
    int i;

    if (!ler_argumentos(argc, argv, &arquivo_enderecos, &alg_pagina, &alg_tlb)){
        return EXIT_FAILURE;
    }

    enderecos = ler_enderecos(arquivo_enderecos, &quantidade);
    if (enderecos == NULL){
        return EXIT_FAILURE;
    }

    if (!inicia_memoria(&memoria, ARQUIVO_BACKING_STORE, alg_pagina)){
        free(enderecos);
        return EXIT_FAILURE;
    }
    
    if (!inicia_tlb(&tlb, alg_tlb)){
        encerra_tlb(&tlb);
        fecha_memoria(&memoria);
        free(enderecos);
        return EXIT_FAILURE;
    }

    for (i = 0; i < quantidade; i++){
        int pagina = numero_pagina(enderecos[i]);
        int offset = numero_offset(enderecos[i]);
        int frame;
        int entrada_tlb;
        int pagina_despejada;
        entrada_tlb = tlb_busca(&tlb, pagina, &frame);

        if (entrada_tlb != TLB_MISS){
            registra_uso(&memoria, frame);
        }else{
            if (!busca_na_tabela(&memoria, pagina, &frame, &pagina_despejada)){
                encerra_tlb(&tlb);
                fecha_memoria(&memoria);
                free(enderecos);
                return EXIT_FAILURE;
            }

            if (pagina_despejada != -1){
                tlb_invalida(&tlb, pagina_despejada);
            }
            entrada_tlb = tlb_insere(&tlb, pagina, frame);
        }

        printf("Virtual address: %d TLB: %d Physical address: %d Value: %d\n", enderecos[i], entrada_tlb, frame * TAMANHO_PAGINA + offset, le_valor(&memoria, frame, offset));
    }

    encerra_tlb(&tlb);
    fecha_memoria(&memoria);
    free(enderecos);

    if (fflush(stdout) != 0 || ferror(stdout)){
        fprintf(stderr, "Error: could not write the output.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}