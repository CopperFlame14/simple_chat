#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_message(const char* prefix, const char* msg) {
    pthread_mutex_lock(&print_mutex);
    printf("\r%s: %s\n> ", prefix, msg);
    fflush(stdout);
    pthread_mutex_unlock(&print_mutex);
}

void* receive_messages(void* arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytes <= 0) {
            print_message("System", "Server disconnected.");
            exit(0);
        }
        print_message("Server", buffer);
    }
    return NULL;
}

void* send_messages(void* arg) {
    char message[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';
        send(sock, message, strlen(message), 0);
        if (strcmp(message, "exit") == 0) {
            print_message("System", "Chat ended by client.");
            exit(0);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) { perror("Socket creation failed"); exit(EXIT_FAILURE); }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address"); close(sock); exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed"); close(sock); exit(EXIT_FAILURE);
    }

    printf("M R KRISHNI 24BCE1704\n");
    printf("Connected to server.\n");

    pthread_t recv_thread, send_thread;
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(sock);
    return 0;
}
