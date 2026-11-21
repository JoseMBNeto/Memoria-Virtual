#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tlb.h"
#include "entrada.h"

static void *thread_busca(void *arg){
    ArgumentoThread *meu = (ArgumentoThread *)arg;
    Tlb *tlb = meu->tlb;
    int i = meu->indice;
    int ultima_rodada = 0;
    int pagina;

    for (;;){
        pthread_mutex_lock(&tlb->trava);
        while (tlb->rodada == ultima_rodada && !tlb->encerrar){
            pthread_cond_wait(&tlb->tem_trabalho, &tlb->trava);
        }

        if (tlb->encerrar){
            pthread_mutex_unlock(&tlb->trava);
            return NULL;
        }
        
        ultima_rodada = tlb->rodada;
        pagina = tlb->pagina_procurada;
        pthread_mutex_unlock(&tlb->trava);
        tlb->achou[i] = tlb->entradas[i].valida && tlb->entradas[i].pagina == pagina;
        pthread_mutex_lock(&tlb->trava);
        tlb->terminadas++;

        if (tlb->terminadas == TAMANHO_TLB){
            pthread_cond_signal(&tlb->terminou);
        }

        pthread_mutex_unlock(&tlb->trava);
    }
}

int inicia_tlb(Tlb *tlb, int algoritmo){
    int i;

    for (i = 0; i < TAMANHO_TLB; i++){
        tlb->entradas[i].pagina = -1;
        tlb->entradas[i].frame = -1;
        tlb->entradas[i].valida = 0;
        tlb->entradas[i].tempo_carga = 0;
        tlb->entradas[i].ultimo_uso = 0;
        tlb->achou[i] = 0;
    }

    tlb->algoritmo = algoritmo;
    tlb->relogio = 0;
    tlb->hits = 0;
    tlb->threads_criadas = 0;
    tlb->sincronizacao_pronta = 0;
    tlb->rodada = 0;
    tlb->terminadas = 0;
    tlb->encerrar = 0;
    tlb->pagina_procurada = -1;

    if (pthread_mutex_init(&tlb->trava, NULL) != 0){
        fprintf(stderr, "Error: could not create the TLB mutex.\n");
        return 0;
    }

    if (pthread_cond_init(&tlb->tem_trabalho, NULL) != 0){
        pthread_mutex_destroy(&tlb->trava);
        fprintf(stderr, "Error: could not create a TLB condition variable.\n");
        return 0;
    }

    if (pthread_cond_init(&tlb->terminou, NULL) != 0){
        pthread_cond_destroy(&tlb->tem_trabalho);
        pthread_mutex_destroy(&tlb->trava);
        fprintf(stderr, "Error: could not create a TLB condition variable.\n");
        return 0;
    }

    tlb->sincronizacao_pronta = 1;

    for (i = 0; i < TAMANHO_TLB; i++){
        tlb->argumentos[i].tlb = tlb;
        tlb->argumentos[i].indice = i;
        if (pthread_create(&tlb->threads[i], NULL, thread_busca, &tlb->argumentos[i]) != 0){
            fprintf(stderr, "Error: could not create TLB thread %d.\n", i);
        }
        tlb->threads_criadas++;
    }

    return 1;
}

void encerra_tlb(Tlb *tlb){
    int i;

    if (!tlb->sincronizacao_pronta){
        return;
    }

    pthread_mutex_lock(&tlb->trava);
    tlb->encerrar = 1;
    pthread_cond_broadcast(&tlb->tem_trabalho);
    pthread_mutex_unlock(&tlb->trava);

    for (i = 0; i < tlb->threads_criadas; i++){
        pthread_join(tlb->threads[i], NULL);
    }

    tlb->threads_criadas = 0;
    pthread_cond_destroy(&tlb->terminou);
    pthread_cond_destroy(&tlb->tem_trabalho);
    pthread_mutex_destroy(&tlb->trava);
    tlb->sincronizacao_pronta = 0;
}

int tlb_busca(Tlb *tlb, int pagina, int *frame){
    int i;

    pthread_mutex_lock(&tlb->trava);
    tlb->pagina_procurada = pagina;
    tlb->terminadas = 0;
    tlb->rodada++;
    pthread_cond_broadcast(&tlb->tem_trabalho);

    while (tlb->terminadas < TAMANHO_TLB){
        pthread_cond_wait(&tlb->terminou, &tlb->trava);
    }

    pthread_mutex_unlock(&tlb->trava);

    for (i = 0; i < TAMANHO_TLB; i++){
        if (tlb->achou[i]){
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