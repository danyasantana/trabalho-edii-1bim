#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX 3
#define MIN 1

typedef struct {
    int matricula;
    long offset;
} Registro;

typedef struct NoArvore {
    int nroChaves;
    Registro chaves[MAX];
    struct NoArvore *filhos[MAX + 1];
    bool folha;
} NoArvore;

typedef struct {
    NoArvore *raiz;
} ArvoreB;
