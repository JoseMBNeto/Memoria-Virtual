#include <stdio.h>
#include <stdlib.h>

#include "memoria.h"

int numero_pagina(int endereco){
    return (endereco & 0xFFFF) >> 8;
}

int numero_offset(int endereco){
    return endereco & 0xFF;
}