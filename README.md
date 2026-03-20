## Sistema de Gerenciamento de Registros com Árvore B
Este projeto é uma implementação de uma Árvore B em linguagem C, desenvolvida para a disciplina de Estrutura de Dados II. O sistema permite o armazenamento persistente de registros (Matrícula, Nome e Telefone) em um arquivo de texto, utilizando a Árvore B em memória para indexação e busca rápida através de offsets.

## 👥 Desenvolvedores
Daniel Alves Santana (danyasantana)

Joao Vitor Luz de Souza (J0luz)

Adam da Victoria Ribeiro (adamribeiro22)

Lucas da Silva Ferreira (Lcs-SF)

## 🛠️ Funcionalidades
O sistema oferece as seguintes operações:

* Cadastrar: Adiciona um novo aluno ao arquivo de registros e insere sua matrícula e posição (offset) na Árvore B.

* Pesquisar: Localiza um registro instantaneamente pela matrícula, utilizando o índice da Árvore B para acessar a posição exata no arquivo.

* Gravar: Exporta a estrutura atual da Árvore B (incluindo endereços de memória e organização dos nós) para um arquivo de texto.

* Persistência: Ao iniciar, o programa lê o arquivo de registros existente e reconstrói a Árvore B automaticamente.

## 🏗️ Estrutura de Dados
A Árvore B foi configurada com os seguintes parâmetros:

* Ordem (MAX): 3

* Mínimo de chaves (MIN): 1

* Registro: Contém a matricula (chave) e o offset (posição no arquivo .txt).

## 📁 Arquivos do Projeto
* main.c: Código-fonte principal contendo a lógica da Árvore B e interface de usuário.

* registros.txt: Banco de dados em texto simples onde os dados completos são salvos.

* arvore.txt: Saída gerada ao escolher a opção "Gravar", detalhando a hierarquia dos nós.
