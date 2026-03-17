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

NoArvore* criarNo(bool folha) {
    NoArvore *no = (NoArvore*)malloc(sizeof(NoArvore));
    no->nroChaves = 0;
    no->folha = folha;
    for (int i = 0; i < MAX + 1; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

void splitFilho(NoArvore *x, int i, NoArvore *y) {
    NoArvore *z = criarNo(y->folha);
    z->nroChaves = MIN;
    
    for (int j = 0; j < MIN; j++) {
        z->chaves[j] = y->chaves[j + MAX / 2];
    }
    
    if (!y->folha) {
        for (int j = 0; j <= MIN; j++) {
            z->filhos[j] = y->filhos[j + MAX / 2 + 1];
        }
    }
    
    y->nroChaves = MIN;
    
    for (int j = x->nroChaves; j >= i + 1; j--) {
        x->filhos[j + 1] = x->filhos[j];
    }
    x->filhos[i + 1] = z;
    
    for (int j = x->nroChaves - 1; j >= i; j--) {
        x->chaves[j + 1] = x->chaves[j];
    }
    x->chaves[i] = y->chaves[MAX / 2];
    x->nroChaves++;
}

void inserirNaoCheio(NoArvore *no, Registro reg) {
    int i = no->nroChaves - 1;
    
    if (no->folha) {
        while (i >= 0 && reg.matricula < no->chaves[i].matricula) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = reg;
        no->nroChaves++;
    } else {
        while (i >= 0 && reg.matricula < no->chaves[i].matricula) {
            i--;
        }
        i++;
        
        if (no->filhos[i]->nroChaves == MAX - 1) {
            splitFilho(no, i, no->filhos[i]);
            if (reg.matricula > no->chaves[i].matricula) {
                i++;
            }
        }
        inserirNaoCheio(no->filhos[i], reg);
    }
}

ArvoreB* criarArvore() {
    ArvoreB *arvore = (ArvoreB*)malloc(sizeof(ArvoreB));
    arvore->raiz = NULL;
    return arvore;
}

void inserir(ArvoreB *arvore, Registro reg) {
    if (arvore->raiz == NULL) {
        arvore->raiz = criarNo(true);
        arvore->raiz->chaves[0] = reg;
        arvore->raiz->nroChaves = 1;
    } else {
        if (arvore->raiz->nroChaves == MAX - 1) {
            NoArvore *s = criarNo(false);
            s->filhos[0] = arvore->raiz;
            splitFilho(s, 0, arvore->raiz);
            inserirNaoCheio(s, reg);
            arvore->raiz = s;
        } else {
            inserirNaoCheio(arvore->raiz, reg);
        }
    }
}

NoArvore* buscar(NoArvore *no, int matricula) {
    if (no == NULL) {
        return NULL;
    }
    
    int i = 0;
    while (i < no->nroChaves && matricula > no->chaves[i].matricula) {
        i++;
    }
    
    if (i < no->nroChaves && matricula == no->chaves[i].matricula) {
        return no;
    }
    
    if (no->folha) {
        return NULL;
    }
    
    return buscar(no->filhos[i], matricula);
}

int buscarIndice(NoArvore *no, int matricula) {
    for (int i = 0; i < no->nroChaves; i++) {
        if (no->chaves[i].matricula == matricula) {
            return i;
        }
    }
    return -1;
}

void liberarArvore(NoArvore *no) {
    if (no == NULL) return;
    
    if (!no->folha) {
        for (int i = 0; i <= no->nroChaves; i++) {
            liberarArvore(no->filhos[i]);
        }
    }
    free(no);
}

void lerRegistro(FILE *fp, long offset, char *nome, char *telefone) {
    char linha[200];
    
    fseek(fp, offset, SEEK_SET);
    
    fgets(linha, sizeof(linha), fp);
    
    fgets(nome, 100, fp);
    nome[strcspn(nome, "\n")] = 0;
    
    fgets(telefone, 20, fp);
    telefone[strcspn(telefone, "\n")] = 0;
}

long adicionarRegistro(FILE *fp, int matricula, char *nome, char *telefone) {
    long offset;
    
    fseek(fp, 0, SEEK_END);
    offset = ftell(fp);
    
    fprintf(fp, "%d\n", matricula);
    fprintf(fp, "%s\n", nome);
    fprintf(fp, "%s\n", telefone);
    fflush(fp);
    
    return offset;
}

void percursoPreOrdem(NoArvore *no, FILE *fp) {
    if (no == NULL) return;
    
    fprintf(fp, "No: ");
    for (int i = 0; i < no->nroChaves; i++) {
        fprintf(fp, "(matricula=%d, offset=%ld)", no->chaves[i].matricula, no->chaves[i].offset);
        if (i < no->nroChaves - 1) fprintf(fp, " | ");
    }
    fprintf(fp, "\n");
    
    if (!no->folha) {
        for (int i = 0; i <= no->nroChaves; i++) {
            percursoPreOrdem(no->filhos[i], fp);
        }
    }
}

int lerArquivoRegistros(FILE *fp, ArvoreB *arvore) {
    char linha[200];
    int count = 0;
    
    rewind(fp);
    
    while (fgets(linha, sizeof(linha), fp) != NULL) {
        int matricula;
        if (sscanf(linha, "%d", &matricula) == 1) {
            char nome[100];
            char telefone[20];
            long offsetAtual = ftell(fp) - strlen(linha);
            
            if (fgets(nome, sizeof(nome), fp) != NULL) {
                nome[strcspn(nome, "\n")] = 0;
            }
            
            if (fgets(telefone, sizeof(telefone), fp) != NULL) {
                telefone[strcspn(telefone, "\n")] = 0;
            }
            
            Registro reg;
            reg.matricula = matricula;
            reg.offset = offsetAtual;
            
            inserir(arvore, reg);
            count++;
        }
    }
    
    return count;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <arquivo_registros> <arquivo_arvore>\n", argv[0]);
        return 1;
    }
    
    char *nomeArquivoRegistros = argv[1];
    char *nomeArquivoArvore = argv[2];
    
    FILE *fpRegistros = fopen(nomeArquivoRegistros, "a+");
    if (fpRegistros == NULL) {
        printf("Erro ao abrir arquivo de registros.\n");
        return 1;
    }
    
    ArvoreB *arvore = criarArvore();
    
    int totalRegistros = lerArquivoRegistros(fpRegistros, arvore);
    printf("Carregados %d registros na arvore B.\n", totalRegistros);
    
    int opcao;
    int matricula;
    char nome[100];
    char telefone[20];
    
    do {
        printf("\n=== Menu ===\n");
        printf("1. Cadastrar\n");
        printf("2. Pesquisar\n");
        printf("3. Gravar\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        
        while (getchar() != '\n');
        
        switch (opcao) {
            case 1: {
                printf("\n--- Cadastrar ---\n");
                printf("Matricula: ");
                scanf("%d", &matricula);
                while (getchar() != '\n');
                printf("Nome: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;
                printf("Telefone: ");
                fgets(telefone, sizeof(telefone), stdin);
                telefone[strcspn(telefone, "\n")] = 0;
                
                long offset = adicionarRegistro(fpRegistros, matricula, nome, telefone);
                
                Registro reg;
                reg.matricula = matricula;
                reg.offset = offset;
                inserir(arvore, reg);
                
                printf("Cadastro realizado com sucesso!\n");
                break;
            }
            case 2: {
                printf("\n--- Pesquisar ---\n");
                printf("Matricula: ");
                scanf("%d", &matricula);
                while (getchar() != '\n');
                
                NoArvore *noEncontrado = buscar(arvore->raiz, matricula);
                
                if (noEncontrado != NULL) {
                    int idx = buscarIndice(noEncontrado, matricula);
                    
                    if (idx != -1) {
                        char nomeBusca[100], telefoneBusca[20];
                        lerRegistro(fpRegistros, noEncontrado->chaves[idx].offset, nomeBusca, telefoneBusca);
                        printf("\nRegistro encontrado:\n");
                        printf("Matricula: %d\n", matricula);
                        printf("Nome: %s\n", nomeBusca);
                        printf("Telefone: %s\n", telefoneBusca);
                    }
                } else {
                    printf("Matricula nao encontrada.\n");
                }
                break;
            }
            case 3: {
                FILE *fpArvore = fopen(nomeArquivoArvore, "w");
                if (fpArvore == NULL) {
                    printf("Erro ao criar arquivo de arvore.\n");
                    break;
                }
                
                fprintf(fpArvore, "Endereco raiz: %p\n", (void*)arvore->raiz);
                fprintf(fpArvore, "=== Nos da Arvore B ===\n");
                percursoPreOrdem(arvore->raiz, fpArvore);
                fclose(fpArvore);
                
                printf("Arvore gravada com sucesso em %s!\n", nomeArquivoArvore);
                break;
            }
            case 4: {
                printf("Saindo...\n");
                break;
            }
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 4);
    
    liberarArvore(arvore->raiz);
    free(arvore);
    fclose(fpRegistros);
    
    return 0;
}
