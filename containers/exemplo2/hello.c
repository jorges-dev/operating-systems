#include <stdio.h>
#include <time.h>

int main() {
    FILE *f = fopen("/shared/hello.txt", "w");

    if (f == NULL) {
        printf("Could not create file.\n");
        return 1;
    }

    time_t now = time(NULL);

    fprintf(f, "=====================================\n");
    fprintf(f, " Hello World from Docker Container\n");
    fprintf(f, " Generated: %s", ctime(&now));
    fprintf(f, "=====================================\n");

    fclose(f);

    printf("\033[1;32m");
    printf("✓ File written to /shared/hello.txt\n");
    printf("\033[0m");

    return 0;
}