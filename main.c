#include <stdio.h>
#include <stdlib.h>
#include "entrada.h"
#include "memoria.h"
#include "tlb.h"
#define ARQUIVO_BACKING_STORE "BACKING_STORE.bin"
#define ARQUIVO_SAIDA "correct.txt"
static Memoria memoria;

int main(int argc, char *argv[]){
    const char *arquivo_enderecos;
    int alg_pagina;
    int alg_tlb;
    int *enderecos;
    int quantidade;
    FILE *saida;
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
    
    inicia_tlb(&tlb, alg_tlb);

    saida = fopen(ARQUIVO_SAIDA, "w");

    if (saida == NULL){
        fprintf(stderr, "Error: could not create %s.\n", ARQUIVO_SAIDA);
        fecha_memoria(&memoria);
        free(enderecos);
        return EXIT_FAILURE;
    }

    for (i = 0; i < quantidade; i++){
        int pagina = numero_pagina(enderecos[i]);
        int offset = numero_offset(enderecos[i]);
        int frame;
        int entrada_tlb;
        entrada_tlb = tlb_busca(&tlb, pagina, &frame);

        if (entrada_tlb != TLB_MISS){
            registra_uso(&memoria, frame);
        }else{
            if (!busca_na_tabela(&memoria, pagina, &frame)){
                fclose(saida);
                remove(ARQUIVO_SAIDA);
                fecha_memoria(&memoria);
                free(enderecos);
                return EXIT_FAILURE;
            }
            entrada_tlb = tlb_insere(&tlb, pagina, frame);
        }

        fprintf(saida, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", enderecos[i], entrada_tlb, frame * TAMANHO_PAGINA + offset, le_valor(&memoria, frame, offset));
    }

    fprintf(saida, "Number of Translated Addresses = %d\n", quantidade);
    fprintf(saida, "Page Faults = %d\n", memoria.page_faults);
    fprintf(saida, "Page Fault Rate = %.3f\n", (double)memoria.page_faults / quantidade);
    fprintf(saida, "TLB Hits = %d\n", tlb.hits);
    fprintf(saida, "TLB Hit Rate = %.3f\n", (double)tlb.hits / quantidade);
    fecha_memoria(&memoria);
    free(enderecos);

    if (ferror(saida) || fclose(saida) != 0){
        fprintf(stderr, "Error: could not write %s.\n", ARQUIVO_SAIDA);
        remove(ARQUIVO_SAIDA);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}