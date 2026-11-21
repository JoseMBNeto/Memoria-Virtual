#ifndef TLB_H
#define TLB_H
#include <pthread.h>
#define TAMANHO_TLB 16
#define TLB_MISS -1

typedef struct EntradaTLB{
    int pagina;
    int frame;
    int valida;
    int tempo_carga;
    int ultimo_uso;
} EntradaTLB;

typedef struct Tlb Tlb;

typedef struct ArgumentoThread{
    Tlb *tlb;
    int indice;
} ArgumentoThread;

struct Tlb{
    EntradaTLB entradas[TAMANHO_TLB];
    int algoritmo;
    int relogio;
    int hits;
    pthread_t threads[TAMANHO_TLB];
    ArgumentoThread argumentos[TAMANHO_TLB];
    int threads_criadas;
    int sincronizacao_pronta;
    pthread_mutex_t trava;
    pthread_cond_t tem_trabalho;
    pthread_cond_t terminou;
    int rodada;
    int terminadas;
    int encerrar;
    int pagina_procurada;
    int achou[TAMANHO_TLB];
};

int inicia_tlb(Tlb *tlb, int algoritmo);
void encerra_tlb(Tlb *tlb);
int tlb_busca(Tlb *tlb, int pagina, int *frame);
int tlb_insere(Tlb *tlb, int pagina, int frame);

#endif