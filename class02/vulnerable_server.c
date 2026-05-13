#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void url_decode(char *src, char *dest) {
    char a, b;

    while (*src) {

        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {

            if (a >= 'a')
                a -= 'a' - 'A';

            if (a >= 'A')
                a = a - 'A' + 10;
            else
                a -= '0';

            if (b >= 'a')
                b -= 'a' - 'A';

            if (b >= 'A')
                b = b - 'A' + 10;
            else
                b -= '0';

            *dest++ = 16 * a + b;
            src += 3;

        } else if (*src == '+') {

            *dest++ = ' ';
            src++;

        } else {

            *dest++ = *src++;
        }
    }

    *dest = '\0';
}

void handle_client(int client_socket) {

    char buffer[BUFFER_SIZE] = {0};
    char command[1024] = {0};
    char decoded[1024] = {0};

    ssize_t bytes_read = read(
        client_socket,
        buffer,
        sizeof(buffer) - 1
    );

    if (bytes_read <= 0) {
        close(client_socket);
        return;
    }

    printf("=====================================\n");
    printf("Requisição recebida:\n%s\n", buffer);

    char *cmd_start = strstr(buffer, "cmd=");

    if (cmd_start != NULL) {

        cmd_start += 4;

        int i = 0;

        while (
            cmd_start[i] != ' ' &&
            cmd_start[i] != '&' &&
            cmd_start[i] != '\0' &&
            i < sizeof(command) - 1
        ) {

            command[i] = cmd_start[i];
            i++;
        }

        command[i] = '\0';

        url_decode(command, decoded);

        printf("Comando decodificado: %s\n", decoded);

        FILE *fp = popen(decoded, "r");

        char output[2048] = {0};

        if (fp != NULL) {

            fread(
                output,
                1,
                sizeof(output) - 1,
                fp
            );

            pclose(fp);

        } else {

            strcpy(output, "Erro ao executar comando\n");
        }

        char response[4096];

        snprintf(
            response,
            sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            output
        );

        write(
            client_socket,
            response,
            strlen(response)
        );

    } else {

        char *response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Use: ?cmd=whoami";

        write(
            client_socket,
            response,
            strlen(response)
        );
    }

    close(client_socket);
}

int main() {

    int server_fd;
    int client_socket;
    int opt = 1;

    struct sockaddr_in address;

    socklen_t addrlen = sizeof(address);

    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd < 0) {

        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(
        server_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    address.sin_family = AF_INET;

    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    address.sin_port = htons(PORT);

    if (
        bind(
            server_fd,
            (struct sockaddr *)&address,
            sizeof(address)
        ) < 0
    ) {

        perror("Erro no bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {

        perror("Erro no listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=====================================\n");
    printf("Servidor vulnerável rodando\n");
    printf("URL: http://127.0.0.1:%d\n", PORT);
    printf("=====================================\n");

    while (1) {

        client_socket = accept(
            server_fd,
            (struct sockaddr *)&address,
            &addrlen
        );

        if (client_socket < 0) {

            perror("Erro no accept");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_fd);

    return 0;
}
