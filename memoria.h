#ifndef MEMORIA_H
#define MEMORIA_H
#include <stdio.h>

#define TAMANHO_PAGINA 256
#define NUM_PAGINAS 256
#define SEM_FRAME -1
#ifndef NUM_FRAMES
#define NUM_FRAMES 128
#endif

#if NUM_FRAMES < 1 || NUM_FRAMES > 256
#error "NUM_FRAMES precisa estar entre 1 e 256"
#endif

typedef struct Memoria{
    int tabela_paginas[NUM_PAGINAS];
    int pagina_do_frame[NUM_FRAMES];
    signed char fisica[NUM_FRAMES][TAMANHO_PAGINA];
    int frames_usados;
    int algoritmo;
    int page_faults;
    FILE *backing_store;
} Memoria;

int numero_pagina(int endereco);
int numero_offset(int endereco);
int inicia_memoria(Memoria *mem, const char *nome_backing_store, int algoritmo);
void fecha_memoria(Memoria *mem);
int busca_na_tabela(Memoria *mem, int pagina, int *frame);
int le_valor(Memoria *mem, int frame, int offset);

#endif