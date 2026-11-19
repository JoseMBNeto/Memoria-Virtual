#ifndef ENTRADA_H
#define ENTRADA_H
#define ALG_FIFO 0
#define ALG_LRU 1

int converte_inteiro(const char *texto, int *resultado);
int ler_argumentos(int argc, char *argv[], const char **arquivo, int *alg_pagina, int *alg_tlb);
int *ler_enderecos(const char *nome_arquivo, int *quantidade);

#endif