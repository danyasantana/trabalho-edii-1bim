/*
 * Arquivo de implementacao da Arvore B
 * Contem todas as funcoes para manipulacao da estrutura de dados
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "arvore-b.h"

/* ============================================================================
 * FUNCOES DE CRIACAO E DESTRUICAO
 * ============================================================================ */

/*
 * criarArvore: Aloca memoria para a estrutura da Arvore B e inicializa a raiz como NULL.
 * Retorna: Ponteiro para a ArvoreB criada, ou NULL em caso de erro.
 */
ArvoreB* criarArvore() {
    ArvoreB *arvore = (ArvoreB*)malloc(sizeof(ArvoreB));
    if (arvore == NULL) return NULL;
    arvore->raiz = NULL;
    return arvore;
}

/*
 * criarNo: Aloca memoria para um novo no da Arvore B.
 * Parametros:
 *   - folha: booleano indicando se o no e uma folha (true) ou interno (false)
 * Retorna: Ponteiro para o NoArvore criado, ou NULL em caso de erro.
 * 
 * Um no folha nao tem filhos, apenas chaves. Um no interno tem filhos.
 */
NoArvore* criarNo(bool folha) {
    NoArvore *no = (NoArvore*)malloc(sizeof(NoArvore));
    if (no == NULL) return NULL;
    no->nroChaves = 0;
    no->folha = folha;
    for (int i = 0; i < MAX; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

/*
 * liberarArvore: Libera recursivamente toda a memoria alocada para a Arvore B.
 * Parametros:
 *   - no: ponteiro para o no raiz (ou sub-no) a ser liberado
 * 
 * A funcao percorre a arvore em profundidade (pos-ordem) liberando primeiro
 * os filhos e depois o proprio no.
 */
void liberarArvore(NoArvore *no) {
    if (no == NULL) return;
    if (!no->folha) {
        for (int i = 0; i <= no->nroChaves; i++) {
            liberarArvore(no->filhos[i]);
        }
    }
    free(no);
}

/* ============================================================================
 * FUNCOES DE INSERCAO
 * ============================================================================ */

/*
 * encontrarPosicao: Encontra a posicao onde uma chave deve ser inserida.
 * Parametros:
 *   - chaves: array de chaves do no
 *   - n: numero atual de chaves no no
 *   - matricula: chave a ser inserida
 * Retorna: Indice onde a chave deve ser inserida (0 a n)
 * 
 * A busca e binaria por natureza: como as chaves estao ordenadas,
 * percorremos da esquerda para direita ate encontrar uma chave maior.
 */
static int encontrarPosicao(Registro *chaves, int n, int matricula) {
    int pos = 0;
    while (pos < n && chaves[pos].matricula < matricula) {
        pos++;
    }
    return pos;
}

/*
 * adicionarChave: Insere uma chave em um no que ainda tem espaco.
 * Parametros:
 *   - no: no onde inserir a chave
 *   - reg: registro contendo a chave
 *   - filhoDir: ponteiro para o filho direito da chave inserida (NULL para folhas)
 * 
 * Desloca as chaves e filhos para a direita para abrir espaco na posicao correta.
 */
static void adicionarChave(NoArvore *no, Registro reg, NoArvore *filhoDir) {
    int pos = encontrarPosicao(no->chaves, no->nroChaves, reg.matricula);
    
    for (int i = no->nroChaves; i > pos; i--) {
        no->chaves[i] = no->chaves[i - 1];
        no->filhos[i + 1] = no->filhos[i];
    }
    no->chaves[pos] = reg;
    no->filhos[pos + 1] = filhoDir;
    no->nroChaves++;
}

/*
 * dividirNo: Divide um no cheio em dois nos menores.
 * Parametros:
 *   - no: no pai que recebera a chave promoted
 *   - pos: posicao do filho a ser dividido no array de filhos do pai
 *   - filho: no cheio a ser dividido
 * Retorna: Ponteiro para o novo no criado
 * 
 * Algoritmo:
 *   1. Cria novo no com a metade direita das chaves
 *   2. Move chaves para o novo no
 *   3. Move filhos para o novo no (se nao for folha)
 *   4. A chave do meio sobe para o pai
 * 
 * Exemplo com MAX=5 (t=3):
 *   Antes: filho com 4 chaves [A,B,C,D]
 *   Depois: filho com 2 chaves [A,B], pai recebe C, novo com 1 chave [D]
 */
static NoArvore* dividirNo(NoArvore *no, int pos, NoArvore *filho) {
    int t = (MAX + 1) / 2;  // Ponto de divisoes (para MAX=5, t=3)
    
    NoArvore *novo = criarNo(filho->folha);
    novo->nroChaves = filho->nroChaves - t;
    
    // Copia chaves da direita para o novo no
    for (int i = 0; i < novo->nroChaves; i++) {
        novo->chaves[i] = filho->chaves[i + t];
    }
    
    // Copia filhos da direita (se nao for folha)
    if (!filho->folha) {
        for (int i = 0; i <= novo->nroChaves; i++) {
            novo->filhos[i] = filho->filhos[i + t];
        }
    }
    
    // Atualiza numero de chaves do filho original
    filho->nroChaves = t - 1;
    
    // Insere novo no como filho do pai
    for (int i = no->nroChaves; i >= pos + 1; i--) {
        no->filhos[i + 1] = no->filhos[i];
    }
    no->filhos[pos + 1] = novo;
    
    // Move chaves do pai para abrir espaco para a chave promoted
    for (int i = no->nroChaves - 1; i >= pos; i--) {
        no->chaves[i + 1] = no->chaves[i];
    }
    // Insere a chave do meio no pai
    no->chaves[pos] = filho->chaves[t - 1];
    no->nroChaves++;
    
    return novo;
}

/*
 * inserirNo: Funcao recursiva auxiliar para insercao em um no nao-cheio.
 * Parametros:
 *   - no: no atual onde inserir
 *   - reg: registro a ser inserido
 * Retorna: 1 se inseriu com sucesso, 0 se a chave ja existe
 * 
 * Para nos folhas: apenas insere a chave na posicao correta.
 * Para nos internos: desce recursivamente ate uma folha,
 * dividindo nos cheios quando necessario.
 */
static int inserirNo(NoArvore *no, Registro reg) {
    int pos = encontrarPosicao(no->chaves, no->nroChaves, reg.matricula);
    
    // Caso seja no folha
    if (no->folha) {
        if (pos < no->nroChaves && no->chaves[pos].matricula == reg.matricula) {
            return 0;  // Chave duplicada
        }
        adicionarChave(no, reg, NULL);
        return 1;
    }
    
    // Verifica se a chave ja existe no no interno
    if (pos < no->nroChaves && no->chaves[pos].matricula == reg.matricula) {
        return 0;  // Chave duplicada
    }
    
    // Verifica se o filho esta cheio e precisa ser dividido
    if (no->filhos[pos]->nroChaves == MAX - 1) {
        dividirNo(no, pos, no->filhos[pos]);
        // Apos dividir, pode ser necessario ajustar a posicao
        if (reg.matricula > no->chaves[pos].matricula) {
            pos++;
        }
    }
    
    return inserirNo(no->filhos[pos], reg);
}

/*
 * inserir: Funcao principal de insercao na Arvore B.
 * Parametros:
 *   - arvore: ponteiro para a arvore
 *   - reg: registro a ser inserido
 * 
 * Se a raiz estiver cheia, cria uma nova raiz e split a antiga.
 * Caso contrario, chama inserirNo para insercao recursiva.
 */
void inserir(ArvoreB *arvore, Registro reg) {
    NoArvore *raiz = arvore->raiz;
    
    // Arvore vazia: cria primeiro no
    if (raiz == NULL) {
        arvore->raiz = criarNo(true);
        arvore->raiz->chaves[0] = reg;
        arvore->raiz->nroChaves = 1;
        return;
    }
    
    // Se a raiz esta cheia, precisamos criar nova raiz
    if (raiz->nroChaves == MAX - 1) {
        NoArvore *novo = criarNo(false);
        novo->filhos[0] = raiz;
        
        dividirNo(novo, 0, raiz);
        
        // Decide em qual filho inserir a nova chave
        int pos = 0;
        if (reg.matricula > novo->chaves[0].matricula) {
            pos++;
        }
        inserirNo(novo->filhos[pos], reg);
        
        arvore->raiz = novo;
    } else {
        inserirNo(raiz, reg);
    }
}

/* ============================================================================
 * FUNCOES DE BUSCA
 * ============================================================================ */

/*
 * buscar: Busca uma chave na Arvore B.
 * Parametros:
 *   - no: no atual da busca
 *   - matricula: chave a ser encontrada
 * Retorna: Ponteiro para o no onde a chave foi encontrada, ou NULL se nao encontrar
 * 
 * Algoritmo:
 *   1. Percorre as chaves do no atual da esquerda para direita
 *   2. Se encontrar a chave, retorna o no atual
 *   3. Se chegou em uma folha e nao encontrou, retorna NULL
 *   4. Senao, desce para o filho apropriado
 */
NoArvore* buscar(NoArvore *no, int matricula) {
    if (no == NULL) return NULL;
    
    int pos = 0;
    while (pos < no->nroChaves && matricula > no->chaves[pos].matricula) {
        pos++;
    }
    
    if (pos < no->nroChaves && matricula == no->chaves[pos].matricula) {
        return no;
    }
    
    if (no->folha) return NULL;
    
    return buscar(no->filhos[pos], matricula);
}

/*
 * buscarIndice: Encontra o indice de uma chave dentro de um no.
 * Parametros:
 *   - no: no onde buscar
 *   - matricula: chave a ser encontrada
 * Retorna: Indice da chave no array (0 a nroChaves-1), ou -1 se nao encontrar
 */
int buscarIndice(NoArvore *no, int matricula) {
    for (int i = 0; i < no->nroChaves; i++) {
        if (no->chaves[i].matricula == matricula) return i;
    }
    return -1;
}

/* ============================================================================
 * FUNCOES DE ARQUIVO DE REGISTROS
 * ============================================================================ */

/*
 * lerRegistro: Le nome e telefone de um registro a partir do offset.
 * Parametros:
 *   - fp: ponteiro para arquivo de registros
 *   - offset: posicao no arquivo onde comeca o nome
 *   - nome: array para receber o nome lido
 *   - telefone: array para receber o telefone lido
 * 
 * IMPORTANTE: O offset aponta para o nome, nao para a matricula.
 * Isso permite acesso direto ao registro sem precisar ler todo o arquivo.
 */
void lerRegistro(FILE *fp, long offset, char *nome, char *telefone) {
    fseek(fp, offset, SEEK_SET);
    
    if (fgets(nome, 100, fp) != NULL) {
        nome[strcspn(nome, "\r\n")] = 0;
    } else {
        nome[0] = '\0';
    }
    
    if (fgets(telefone, 20, fp) != NULL) {
        telefone[strcspn(telefone, "\r\n")] = 0;
    } else {
        telefone[0] = '\0';
    }
}

/*
 * adicionarRegistro: Adiciona um novo registro ao arquivo de registros.
 * Parametros:
 *   - fp: ponteiro para arquivo de registros
 *   - matricula: numero de matricula
 *   - nome: nome do aluno
 *   - telefone: telefone do aluno
 * Retorna: Offset onde comeca o nome (para acesso direto)
 * 
 * Formato do arquivo:
 *   <matricula>\n
 *   <nome>\n
 *   <telefone>\n
 */
long adicionarRegistro(FILE *fp, int matricula, char *nome, char *telefone) {
    long offset;
    
    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%d\n", matricula);
    offset = ftell(fp);  // Offset apos gravar matricula = inicio do nome
    fprintf(fp, "%s\n", nome);
    fprintf(fp, "%s\n", telefone);
    fflush(fp);
    
    return offset;
}

/*
 * lerArquivoRegistros: Le todos os registros de um arquivo e insere na arvore.
 * Parametros:
 *   - fp: ponteiro para arquivo de registros
 *   - arvore: arvore onde inserir os registros
 * Retorna: Numero de registros inseridos
 * 
 * O arquivo deve ter o formato:
 *   <matricula>\n
 *   <nome>\n
 *   <telefone>\n
 *   (repetir...)
 */
int lerArquivoRegistros(FILE *fp, ArvoreB *arvore) {
    char linha[256];
    int count = 0;

    if (fp == NULL) return 0;
    rewind(fp);

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        int matricula;
        if (sscanf(linha, "%d", &matricula) == 1) {
            char nome[100] = "";
            char telefone[20] = "";
            long offsetNome = ftell(fp);

            if (fgets(nome, sizeof(nome), fp) != NULL) {
                nome[strcspn(nome, "\r\n")] = 0;
            }
            if (fgets(telefone, sizeof(telefone), fp) != NULL) {
                telefone[strcspn(telefone, "\r\n")] = 0;
            }

            Registro reg = {matricula, offsetNome};
            inserir(arvore, reg);
            count++;
        }
    }
    
    return count;
}

/* ============================================================================
 * FUNCOES PARA SALVAR ARVORE EM ARQUIVO
 * ============================================================================ */

/*
 * contarNos: Conta o numero total de nos na arvore.
 * Parametros:
 *   - no: no raiz (ou sub-raiz)
 * Retorna: Quantidade de nos na sub-arvore
 */
static int contarNos(NoArvore *no) {
    if (no == NULL) return 0;
    int count = 1;
    if (!no->folha) {
        for (int i = 0; i <= no->nroChaves; i++) {
            count += contarNos(no->filhos[i]);
        }
    }
    return count;
}

/*
 * coletarNos: Coleta ponteiros de todos os nos em um array.
 * Parametros:
 *   - no: no atual
 *   - nos: array onde armazenar os ponteiros
 *   - idx: ponteiro para indice atual no array
 * 
 * A coleta e feita em profundidade (pre-ordem).
 */
static void coletarNos(NoArvore *no, NoArvore **nos, int *idx) {
    if (no == NULL) return;
    nos[*idx] = no;
    (*idx)++;
    if (!no->folha) {
        for (int i = 0; i <= no->nroChaves; i++) {
            coletarNos(no->filhos[i], nos, idx);
        }
    }
}

/*
 * encontrarIndiceNo: Encontra o indice de um no no array de nos.
 * Parametros:
 *   - nos: array de ponteiros de nos
 *   - total: numero total de nos
 *   - alvo: no a ser encontrado
 * Retorna: Indice do no no array, ou -1 se nao encontrado
 */
static int encontrarIndiceNo(NoArvore **nos, int total, NoArvore *alvo) {
    for (int i = 0; i < total; i++) {
        if (nos[i] == alvo) return i;
    }
    return -1;
}

/*
 * salvarArvoreTexto: Salva a estrutura da Arvore B em arquivo texto.
 * Parametros:
 *   - arvore: arvore a ser salva
 *   - nomeArquivo: nome do arquivo de saida
 * Retorna: 1 em caso de sucesso, 0 em caso de erro
 * 
 * Formato do arquivo:
 *   Linha 1: node_<indice_raiz>
 *   Linhas seguintes: node_<i> <nroChaves> <chaves> <filhos>
 * 
 * Exemplo:
 *   node_0
 *   node_0 2 1001 5 1002 33 -1 -1 -1 -1 null null null null null
 *   node_1 1 1003 63 -1 -1 -1 -1 -1 -1 null null null null null
 */
int salvarArvoreTexto(ArvoreB *arvore, const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "w");
    if (fp == NULL) return 0;
    
    if (arvore->raiz == NULL) {
        fprintf(fp, "null\n");
        fclose(fp);
        return 1;
    }
    
    // Conta e coleta todos os nos
    int totalNos = contarNos(arvore->raiz);
    NoArvore **nos = (NoArvore**)malloc(sizeof(NoArvore*) * totalNos);
    int idx = 0;
    coletarNos(arvore->raiz, nos, &idx);
    
    // Primeira linha: indice da raiz
    fprintf(fp, "node_%d\n", encontrarIndiceNo(nos, totalNos, arvore->raiz));
    
    // Resto: informacoes de cada no
    for (int i = 0; i < totalNos; i++) {
        NoArvore *no = nos[i];
        fprintf(fp, "node_%d %d", i, no->nroChaves);
        
        // Imprime as chaves existentes
        for (int j = 0; j < no->nroChaves; j++) {
            fprintf(fp, " %d %ld", no->chaves[j].matricula, no->chaves[j].offset);
        }
        // Preenche com -1 ate completar MAX-1 posicoes
        for (int j = no->nroChaves; j < MAX - 1; j++) {
            fprintf(fp, " -1 -1");
        }
        
        // Imprime os filhos
        if (no->folha) {
            for (int j = 0; j < MAX; j++) {
                fprintf(fp, " null");
            }
        } else {
            for (int j = 0; j <= no->nroChaves; j++) {
                int childIdx = encontrarIndiceNo(nos, totalNos, no->filhos[j]);
                fprintf(fp, " node_%d", childIdx);
            }
            for (int j = no->nroChaves + 1; j < MAX; j++) {
                fprintf(fp, " null");
            }
        }
        fprintf(fp, "\n");
    }
    
    free(nos);
    fclose(fp);
    return 1;
}
