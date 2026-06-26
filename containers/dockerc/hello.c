#include <stdio.h>

int main() {
    printf("\033[1;36m");
    printf("╔══════════════════════════════════════╗\n");
    printf("║                                      ║\n");
    printf("║         HELLO WORLD IN DOCKER        ║\n");
    printf("║                                      ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("\033[0m");

    printf("\n");
    printf("\033[1;32m");
    printf("Container executado com sucesso!\n");
    printf("\033[0m");

    return 0;
}