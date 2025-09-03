#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int client_socket;

void* receive_messages(void* arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            printf("\nClient disconnected.\n");
            exit(0);
        }
        printf("\nClient: %s\nServer: ", buffer);
        fflush(stdout);
    }
    return NULL;
}

void* send_messages(void* arg) {
    char message[BUFFER_SIZE];
    while (1) {
        printf("Server: ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';
        send(client_socket, message, strlen(message), 0);
        if (strcmp(message, "exit") == 0) {
            printf("Chat ended by server.\n");
            exit(0);
        }
    }
    return NULL;
}

int main() {
    printf("M R KRISHNI 24BCE1704\n");

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("Socket creation failed"); exit(EXIT_FAILURE); }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed"); close(server_fd); exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed"); close(server_fd); exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (client_socket < 0) { perror("Accept failed"); close(server_fd); exit(EXIT_FAILURE); }

    printf("Connection established with client.\n");

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(client_socket);
    close(server_fd);
    return 0;
}
