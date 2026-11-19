#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"

int numero_pagina(int endereco){
    return (endereco & 0xFFFF) >> 8;
}

int numero_offset(int endereco){
    return endereco & 0xFF;
}

int inicia_memoria(Memoria *mem, const char *nome_backing_store, int algoritmo){
    int i;

    for (i = 0; i < NUM_PAGINAS; i++){
        mem->tabela_paginas[i] = SEM_FRAME;
    }
    for (i = 0; i < NUM_FRAMES; i++){
        mem->pagina_do_frame[i] = -1;
    }

    mem->frames_usados = 0;
    mem->algoritmo = algoritmo;
    mem->page_faults = 0;
    mem->backing_store = fopen(nome_backing_store, "rb");

    if (mem->backing_store == NULL){
        fprintf(stderr, "Error: could not open %s (it does not exist or cannot be read).\n", nome_backing_store);
        return 0;
    }

    return 1;
}

void fecha_memoria(Memoria *mem){
    if (mem->backing_store != NULL){
        fclose(mem->backing_store);
        mem->backing_store = NULL;
    }
}

static int escolhe_frame(Memoria *mem){
    if (mem->frames_usados < NUM_FRAMES){
        return mem->frames_usados++;
    }

    return -1;
}

static int carrega_pagina(Memoria *mem, int pagina, int frame){
    if (fseek(mem->backing_store, (long)pagina * TAMANHO_PAGINA, SEEK_SET) != 0){
        return 0;
    }
    if (fread(mem->fisica[frame], 1, TAMANHO_PAGINA, mem->backing_store) != TAMANHO_PAGINA){
        return 0;
    }
    return 1;
}

int busca_na_tabela(Memoria *mem, int pagina, int *frame){
    int novo_frame;

    if (mem->tabela_paginas[pagina] != SEM_FRAME){
        *frame = mem->tabela_paginas[pagina];
        return 1;
    }

    mem->page_faults++;

    novo_frame = escolhe_frame(mem);
    if (novo_frame < 0){
        fprintf(stderr, "Error: physical memory is full (page replacement not implemented yet).\n");
        return 0;
    }

    if (!carrega_pagina(mem, pagina, novo_frame)){
        fprintf(stderr, "Error: could not read page %d from the backing store (the file is too small or damaged).\n", pagina);
        return 0;
    }

    mem->tabela_paginas[pagina] = novo_frame;
    mem->pagina_do_frame[novo_frame] = pagina;
    *frame = novo_frame;
    return 1;
}

int le_valor(Memoria *mem, int frame, int offset){
    return mem->fisica[frame][offset];
}