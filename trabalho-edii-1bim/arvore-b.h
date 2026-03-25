/*
 * Arquivo de cabecalho da Arvore B
 * Contem as definicoes de tipos e declaracoes das funcoes publicas
 */

#ifndef ARVORE_B
#define ARVORE_B

#include <stdio.h>
#include <stdbool.h>

/*
 * MAX: Ordem da Arvore B
 * Define o numero maximo de filhos que um no pode ter
 * Com MAX=5, cada no pode ter ate 4 chaves
 */
#define MAX 5

/*
 * MIN: Numero minimo de chaves em um no (exceto raiz)
 * MIN = MAX / 2 = 2 (para MAX=5)
 * Garantia pela definicao da Arvore B
 */
#define MIN (MAX / 2)

/* ============================================================================
 * ESTRUTURAS DE DADOS
 * ============================================================================ */

/*
 * Registro: Estrutura que representa um registro de aluno
 * - matricula: numero unico de identificacao do aluno
 * - offset: posicao no arquivo de registros onde comeca o nome
 * 
 * A matricula e usada como chave para ordenacao na arvore
 * O offset permite acesso direto aos dados completos (nome, telefone)
 */
typedef struct {
    int matricula;   // Chave primaria para busca na arvore
    long offset;     // Posicao no arquivo para acesso direto aos dados
} Registro;

/*
 * NoArvore: Estrutura de um no da Arvore B
 * - nroChaves: quantidade atual de chaves no no
 * - chaves: array de registros (MAX-1 posicoes)
 * - filhos: array de ponteiros para nos filhos (MAX posicoes)
 * - folha: indica se o no e uma folha (nao tem filhos)
 */
typedef struct NoArvore {
    int nroChaves;                       // Numero atual de chaves
    Registro chaves[MAX - 1];            // Array de chaves (maximo MAX-1)
    struct NoArvore *filhos[MAX];        // Array de ponteiros para filhos
    bool folha;                          // Verdadeiro se for no folha
} NoArvore;

/*
 * ArvoreB: Estrutura principal que representa a Arvore B
 * - raiz: ponteiro para o no raiz da arvore
 */
typedef struct {
    NoArvore *raiz;                     // Ponteiro para a raiz da arvore
} ArvoreB;

/* ============================================================================
 * DECLARACOES DAS FUNCOES
 * ============================================================================ */

/* Funcoes de criacao e destruicao */
ArvoreB* criarArvore();                  // Cria uma nova arvore vazia
NoArvore* criarNo(bool folha);          // Cria um novo no
void liberarArvore(NoArvore *no);       // Libera toda a memoria da arvore

/* Funcoes de manipulacao */
void inserir(ArvoreB *arvore, Registro reg);  // Insere um registro na arvore

/* Funcoes de busca */
NoArvore* buscar(NoArvore *no, int matricula);      // Busca uma matricula na arvore
int buscarIndice(NoArvore *no, int matricula);      // Encontra indice de chave no no

/* Funcoes de arquivo de registros */
void lerRegistro(FILE *fp, long offset, char *nome, char *telefone);  // Le dados pelo offset
long adicionarRegistro(FILE *fp, int matricula, char *nome, char *telefone);  // Adiciona ao arquivo
int lerArquivoRegistros(FILE *fp, ArvoreB *arvore);  // Carrega registros do arquivo para arvore

/* Funcoes de persistencia da arvore */
int salvarArvoreTexto(ArvoreB *arvore, const char *nomeArquivo);  // Salva arvore em arquivo texto

#endif
