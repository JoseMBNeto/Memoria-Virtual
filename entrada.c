#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "entrada.h"
#define TAMANHO_LINHA 4096
#define LINHA_OK 1
#define FIM_DO_ARQUIVO 0
#define LINHA_GRANDE -1

int converte_inteiro(const char *texto, int *resultado){
    int valor = 0;
    int i;

    if (texto == NULL || texto[0] == '\0'){
        return 0;
    }

    for (i = 0; texto[i] != '\0'; i++){
        int digito;

        if (texto[i] < '0' || texto[i] > '9'){
            return 0;
        }

        digito = texto[i] - '0';

        if (valor > (INT_MAX - digito) / 10){
            return 0;
        }

        valor = valor * 10 + digito;
    }

    *resultado = valor;
    return 1;
}

static int converte_algoritmo(const char *texto, int *algoritmo){
    if (strcmp(texto, "fifo") == 0){
        *algoritmo = ALG_FIFO;
        return 1;
    }
    if (strcmp(texto, "lru") == 0){
        *algoritmo = ALG_LRU;
        return 1;
    }
    return 0;
}

int ler_argumentos(int argc, char *argv[], const char **arquivo, int *alg_pagina, int *alg_tlb){
    if (argc != 4){
        fprintf(stderr, "Error: expected 3 arguments, but got %d.\n", argc - 1);
        fprintf(stderr, "Usage: %s <addresses file> <fifo|lru (page replacement)> <fifo|lru (TLB replacement)>\n", argv[0]);
        return 0;
    }

    if (!converte_algoritmo(argv[2], alg_pagina)){
        fprintf(stderr, "Error: invalid page replacement algorithm \"%s\" (expected fifo or lru).\n", argv[2]);
        return 0;
    }

    if (!converte_algoritmo(argv[3], alg_tlb)){
        fprintf(stderr, "Error: invalid TLB replacement algorithm \"%s\" (expected fifo or lru).\n", argv[3]);
        return 0;
    }

    *arquivo = argv[1];
    return 1;
}

static int le_linha(FILE *arquivo, char *buffer, int tamanho){
    size_t len;

    if (fgets(buffer, tamanho, arquivo) == NULL){
        return FIM_DO_ARQUIVO;
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
        len--;
    }else{
        int proximo = fgetc(arquivo);
        if (proximo != EOF){
            return LINHA_GRANDE;
        }
    }

    if (len > 0 && buffer[len - 1] == '\r'){
        buffer[len - 1] = '\0';
    }

    return LINHA_OK;
}

static char *remove_espacos(char *linha){
    size_t len;

    while (*linha == ' ' || *linha == '\t'){
        linha++;
    }

    len = strlen(linha);

    while (len > 0 && (linha[len - 1] == ' ' || linha[len - 1] == '\t')){
        linha[len - 1] = '\0';
        len--;
    }

    return linha;
}

static int passada_enderecos(FILE *arquivo, const char *nome_arquivo, int *enderecos, int capacidade){
    char linha[TAMANHO_LINHA];
    int num_linha = 0;
    int quantidade = 0;
    int primeira_vazia = 0;
    int status;

    while ((status = le_linha(arquivo, linha, TAMANHO_LINHA)) != FIM_DO_ARQUIVO){
        char *numero;
        int valor;
        num_linha++;

        if (status == LINHA_GRANDE){
            fprintf(stderr, "Error: %s line %d: line is too long (limit is %d characters).\n", nome_arquivo, num_linha, TAMANHO_LINHA - 2);
            return -1;
        }

        numero = remove_espacos(linha);

        if (numero[0] == '\0'){
            if (primeira_vazia == 0){
                primeira_vazia = num_linha;
            }
            continue;
        }

        if (primeira_vazia != 0){
            fprintf(stderr, "Error: %s line %d: empty line (empty lines are only allowed at the end of the file).\n", nome_arquivo, primeira_vazia);
            return -1;
        }

        if (!converte_inteiro(numero, &valor)){
            fprintf(stderr, "Error: %s line %d: \"%s\" is not a valid address (expected one non-negative integer per line).\n", nome_arquivo, num_linha, numero);
            return -1;
        }

        if (enderecos != NULL){
            if (quantidade >= capacidade){
                fprintf(stderr, "Error: %s changed while it was being read.\n", nome_arquivo);
                return -1;
            }

            enderecos[quantidade] = valor;
        }

        quantidade++;
    }

    if (ferror(arquivo)){
        fprintf(stderr, "Error: could not read %s.\n", nome_arquivo);
        return -1;
    }

    return quantidade;
}

int *ler_enderecos(const char *nome_arquivo, int *quantidade){
    FILE *arquivo;
    int *enderecos;
    int n;
    int conferencia;

    arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL){
        fprintf(stderr, "Error: could not open %s (it does not exist or cannot be read).\n", nome_arquivo);
        return NULL;
    }

    n = passada_enderecos(arquivo, nome_arquivo, NULL, 0);

    if (n < 0){
        fclose(arquivo);
        return NULL;
    }
    if (n == 0){
        fprintf(stderr, "Error: %s has no addresses.\n", nome_arquivo);
        fclose(arquivo);
        return NULL;
    }

    enderecos = malloc(n * sizeof(int));
    if (enderecos == NULL){
        fprintf(stderr, "Error: not enough memory.\n");
        fclose(arquivo);
        return NULL;
    }

    rewind(arquivo);
    conferencia = passada_enderecos(arquivo, nome_arquivo, enderecos, n);
    fclose(arquivo);

    if (conferencia != n){
        if (conferencia >= 0){
            fprintf(stderr, "Error: %s changed while it was being read.\n", nome_arquivo);
        }
        free(enderecos);
        return NULL;
    }

    *quantidade = n;
    return enderecos;
}