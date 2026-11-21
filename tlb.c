#include <stdio.h>
#include <stdlib.h>
#include "tlb.h"
#include "entrada.h"

void inicia_tlb(Tlb *tlb, int algoritmo){
    int i;

    for (i = 0; i < TAMANHO_TLB; i++){
        tlb->entradas[i].pagina = -1;
        tlb->entradas[i].frame = -1;
        tlb->entradas[i].valida = 0;
        tlb->entradas[i].tempo_carga = 0;
        tlb->entradas[i].ultimo_uso = 0;
    }

    tlb->algoritmo = algoritmo;
    tlb->relogio = 0;
    tlb->hits = 0;
}

int tlb_busca(Tlb *tlb, int pagina, int *frame){
    int i;

    for (i = 0; i < TAMANHO_TLB; i++){
        if (tlb->entradas[i].valida && tlb->entradas[i].pagina == pagina){
            tlb->hits++;
            tlb->relogio++;
            tlb->entradas[i].ultimo_uso = tlb->relogio;
            *frame = tlb->entradas[i].frame;
            return i;
        }
    }

    return TLB_MISS;
}

static int escolhe_entrada(Tlb *tlb){
    int vitima = 0;
    int i;

    for (i = 0; i < TAMANHO_TLB; i++){
        if (!tlb->entradas[i].valida){
            return i;
        }
    }

    for (i = 1; i < TAMANHO_TLB; i++){
        if (tlb->algoritmo == ALG_FIFO){
            if (tlb->entradas[i].tempo_carga < tlb->entradas[vitima].tempo_carga){
                vitima = i;
            }
        }else{
            if (tlb->entradas[i].ultimo_uso < tlb->entradas[vitima].ultimo_uso){
                vitima = i;
            }
        }
    }
    
    return vitima;
}

int tlb_insere(Tlb *tlb, int pagina, int frame){
    int i = escolhe_entrada(tlb);

    tlb->relogio++;
    tlb->entradas[i].pagina = pagina;
    tlb->entradas[i].frame = frame;
    tlb->entradas[i].valida = 1;
    tlb->entradas[i].tempo_carga = tlb->relogio;
    tlb->entradas[i].ultimo_uso = tlb->relogio;

    return i;
}