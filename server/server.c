#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <stdbool.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

struct User {
    char username[32];
    char password[32];
};

struct User users[] = {
    {"user1", "password1"},
    {"user2", "password2"},
    {"user3", "password3"}
};

int authenticate(char *username, char *password) {
    int numUsers = sizeof(users) / sizeof(struct User);
    for (int i = 0; i < numUsers; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    WSADATA wsa_data;
    bool estado = false;
    int cont = 2;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Error al inicializar Winsock\n");
        return 1;
    }

    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    // Crear el socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error al crear el socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Configurar la direccion del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(3000);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al vincular el socket");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Error al escuchar");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado. Esperando conexiones...\n");

    while (1) {
        // Aceptar una conexión entrante
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Error al aceptar la conexión");
            continue;
        }

        printf("Nueva conexión aceptada desde %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Recibir las credenciales del cliente
        ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            perror("Error al recibir datos");
            closesocket(client_fd);
            continue;
        }
        buffer[bytes_received] = '\0';
        
        if(!estado){
            // Separar el nombre de usuario y la contraseña
            char *username = strtok(buffer, ":");
            char *password = strtok(NULL, ":");

            if (username == NULL || password == NULL) {
                printf("Formato de credenciales inválido\n");
                const char *response = "Formato de credenciales inválido";
                send(client_fd, response, strlen(response), 0);
                closesocket(client_fd);
                continue;
            }

            // Autenticar al usuario
            if (authenticate(username, password)) {
                printf("Usuario autenticado: %s\n", username);
                char numero_str[20];
                sprintf(numero_str, "%d", cont);
                cont--;    
                send(client_fd, numero_str, strlen(numero_str), 0);
            } else {
                printf("Credenciales inválidas para: %s\n", username);
                const char *response = "0";
                send(client_fd, response, strlen(response), 0);
            }

            if(cont <= 0 ){
                estado = true;

            }

        }

        closesocket(client_fd);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}