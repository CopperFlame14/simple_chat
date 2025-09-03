#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  printf("M R KRISHNI 24BCE1704\n");
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    while (1) {
        printf("Client: ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';

        send(sock, message, strlen(message), 0);

        if (strcmp(message, "exit") == 0) {
            printf("Chat ended by client.\n");
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}
