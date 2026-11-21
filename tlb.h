#ifndef TLB_H
#define TLB_H
#define TAMANHO_TLB 16
#define TLB_MISS -1

typedef struct EntradaTLB{
    int pagina;
    int frame;
    int valida;
    int tempo_carga;
    int ultimo_uso;
} EntradaTLB;

typedef struct Tlb{
    EntradaTLB entradas[TAMANHO_TLB];
    int algoritmo;
    int relogio;
    int hits;
} Tlb;

void inicia_tlb(Tlb *tlb, int algoritmo);
int tlb_busca(Tlb *tlb, int pagina, int *frame);
int tlb_insere(Tlb *tlb, int pagina, int frame);

#endif