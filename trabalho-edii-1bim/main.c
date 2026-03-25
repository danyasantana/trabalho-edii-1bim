/*
 * Programa principal - Sistema de cadastro de alunos com Arvore B
 * 
 * Este programa implementa um sistema de cadastro que utiliza Arvore B
 * para indexacao e arquivos texto para persistencia de dados.
 * 
 * Uso: ./programa <arquivo_registros> <arquivo_arvore>
 *   - arquivo_registros: arquivo texto com os dados dos alunos
 *   - arquivo_arvore: arquivo onde sera salva a estrutura da arvore
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "arvore-b.h"

int main(int argc, char *argv[]) {
    /*
     * Verifica se os argumentos foram passados corretamente
     * argc deve ser 3: nome do programa + 2 arquivos
     */
    if (argc != 3) {
        printf("Uso: %s <arquivo_registros> <arquivo_arvore>\n", argv[0]);
        return 1;
    }
    
    char *nomeArquivoRegistros = argv[1];  // Arquivo com dados dos alunos
    char *nomeArquivoArvore = argv[2];     // Arquivo para salvar a arvore
    
    /*
     * Abre o arquivo de registros em modo "a+" (leitura e anexacao)
     * Se o arquivo nao existir, sera criado
     */
    FILE *fpRegistros = fopen(nomeArquivoRegistros, "a+");
    if (fpRegistros == NULL) {
        printf("Erro ao abrir arquivo de registros.\n");
        return 1;
    }
    
    /*
     * Cria a arvore B em memoria principal
     * e carrega os registros existentes do arquivo
     */
    ArvoreB *arvore = criarArvore();
    int totalRegistros = lerArquivoRegistros(fpRegistros, arvore);
    printf("Carregados %d registros na arvore B.\n", totalRegistros);

    int opcao;
    int matricula;
    char nome[100];
    char telefone[20];
    
    /*
     * Loop principal do menu
     * Exibe as opcoes ate o usuario escolher sair (opcao 4)
     */
    do {
        printf("\n=== Menu ===\n");
        printf("1. Cadastrar\n");
        printf("2. Pesquisar\n");
        printf("3. Gravar\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        fflush(stdout);  // Garante que a mensagem apareca antes do scanf
        scanf("%d", &opcao);
        
        while (getchar() != '\n');  // Limpa o buffer do teclado
        
        switch (opcao) {
            case 1: {
                /*
                 * CADASTRAR
                 * Solicita matricula, nome e telefone
                 * Adiciona ao arquivo de registros
                 * Insere na arvore B
                 */
                printf("\n--- Cadastrar ---\n");
                printf("Matricula: ");
                fflush(stdout);
                scanf("%d", &matricula);
                getchar();  // Consome o newline
                printf("Nome: ");
                fflush(stdout);
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = 0;  // Remove o newline
                printf("Telefone: ");
                fflush(stdout);
                fgets(telefone, sizeof(telefone), stdin);
                telefone[strcspn(telefone, "\n")] = 0;
                
                /*
                 * Adiciona ao arquivo de registros e obtem o offset
                 * O offset permite acesso direto ao registro
                 */
                long offset = adicionarRegistro(fpRegistros, matricula, nome, telefone);
                
                Registro reg;
                reg.matricula = matricula;
                reg.offset = offset;
                inserir(arvore, reg);
                
                printf("Cadastro realizado com sucesso!\n");
                break;
            }
            case 2: {
                /*
                 * PESQUISAR
                 * Solicita a matricula
                 * Busca na arvore B (busca em memoria)
                 * Se encontrar, usa o offset para ler os dados do arquivo
                 * 
                 * IMPORTANTE: O acesso ao arquivo e DIRETO via offset
                 * Nao ha pesquisa sequencial no arquivo
                 */
                printf("\n--- Pesquisar ---\n");
                printf("Matricula: ");
                fflush(stdout);
                scanf("%d", &matricula);
                getchar();
                
                /*
                 * Busca o no onde a matricula esta armazenada
                 */
                NoArvore *noEncontrado = buscar(arvore->raiz, matricula);
                
                if (noEncontrado != NULL) {
                    /*
                     * Encontrou a matricula na arvore
                     * Usa o offset para ler nome e telefone diretamente
                     */
                    int idx = buscarIndice(noEncontrado, matricula);
                    
                    if (idx != -1) {
                        char nomeBusca[100], telefoneBusca[20];
                        /*
                         * Acesso direto ao registro pelo offset
                         * Esta funcao posiciona o ponteiro do arquivo
                         * exatamente onde estao os dados, sem precisar
                         * ler todo o arquivo
                         */
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
                /*
                 * GRAVAR
                 * Salva a estrutura da arvore B em arquivo texto
                 * A primeira linha contem o endereco da raiz
                 * As demais linhas contem os nos da arvore
                 */
                if (salvarArvoreTexto(arvore, nomeArquivoArvore)) {
                    printf("Arvore gravada com sucesso em %s!\n", nomeArquivoArvore);
                } else {
                    printf("Erro ao gravar arvore em %s.\n", nomeArquivoArvore);
                }
                break;
            }
            case 4: {
                /*
                 * SAIR
                 * Libera a memoria da arvore
                 * Fecha os arquivos
                 * Encerra o programa
                 */
                printf("Saindo...\n");
                break;
            }
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 4);
    
    /*
     * Limpeza final:
     * Libera a memoria alocada para a arvore
     */
    liberarArvore(arvore->raiz);
    free(arvore);
    fclose(fpRegistros);
    
    return 0;
}
