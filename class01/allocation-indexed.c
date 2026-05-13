#include <stdio.h>

#define MAX_BLOCOS 50
#define MAX_ARQUIVOS 10
#define MAX_BLOCOS_ARQ 10

typedef struct {
    char nome[20];
    int indice;
    int blocos[MAX_BLOCOS_ARQ];
    int qtdBlocos;
} Arquivo;

int main() {
    int disco[MAX_BLOCOS] = {0};
    Arquivo arquivos[MAX_ARQUIVOS];
    int qtdArquivos = 0;
    int opcao;

    do {
        printf("\n1 - Criar arquivo");
        printf("\n2 - Listar arquivos");
        printf("\n0 - Sair");
        printf("\nOpcao: ");
        scanf("%d", &opcao);

        if (opcao == 1) {
            Arquivo arq;

            printf("Nome do arquivo: ");
            scanf("%s", arq.nome);

            printf("Quantidade de blocos do arquivo: ");
            scanf("%d", &arq.qtdBlocos);

            printf("Bloco de indice: ");
            scanf("%d", &arq.indice);

            if (disco[arq.indice] == 1) {
                printf("Bloco de indice ja ocupado!\n");
                continue;
            }

            disco[arq.indice] = 1;

            printf("Digite os blocos de dados:\n");
            for (int i = 0; i < arq.qtdBlocos; i++) {
                printf("Bloco %d: ", i + 1);
                scanf("%d", &arq.blocos[i]);

                if (disco[arq.blocos[i]] == 1) {
                    printf("Bloco ja ocupado! Operacao cancelada.\n");
                    disco[arq.indice] = 0;
                    i = arq.qtdBlocos;
                    continue;
                }

                disco[arq.blocos[i]] = 1;
            }

            arquivos[qtdArquivos] = arq;
            qtdArquivos++;

            printf("Arquivo criado com alocacao indexada!\n");
        }

        else if (opcao == 2) {
            printf("\nArquivos cadastrados:\n");

            for (int i = 0; i < qtdArquivos; i++) {
                printf("\nArquivo: %s", arquivos[i].nome);
                printf("\nBloco de indice: %d", arquivos[i].indice);
                printf("\nBlocos de dados: ");

                for (int j = 0; j < arquivos[i].qtdBlocos; j++) {
                    printf("%d ", arquivos[i].blocos[j]);
                }

                printf("\n");
            }
        }

    } while (opcao != 0);

    return 0;
}
