#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
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

int stringToInt(char* str) {
    return (str != NULL && str[0] != '\0') ? atoi(str) : 0;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    char buffer[BUFFER_SIZE];

    // Crear el socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la direccion del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5060);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al vincular el socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Error al escuchar");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado. Esperando conexiones...\n");

    bool autentificados = false;
    int cont = 2;
    char *leTocaA = "1";
    int tablero[6][7];
    int tableroMensaje[6][7]; // Inicializar la matriz con ceros
    memset(tableroMensaje, 8, sizeof(tablero)); // Asegurarse de que todos los elementos sean 0
    
    // Inicializando el tablero con ceros
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            tablero[i][j] = 0;
        }
    }

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
            close(client_fd);
            continue;
        }
        buffer[bytes_received] = '\0';
        printf("Recibi: %s\n", buffer);
        if(!autentificados){
            // Separar el nombre de usuario y la contraseña
            char *username = strtok(buffer, ":");
            char *password = strtok(NULL, ":");

            if (username == NULL || password == NULL) {
                printf("Formato de credenciales inválido\n");
                const char *response = "Formato de credenciales inválido";
                send(client_fd, response, strlen(response), 0);
                close(client_fd);
                continue;
            }

            // Autenticar al usuario
            if(strcmp(username, "2") == 0){
                printf("Envie: No\n");
                const char *response = "No";
                send(client_fd, response, strlen(response), 0);
            }
            else if (authenticate(username, password)) {
                printf("Usuario autenticado: %s\n", username);
                char numero_str[20];
                sprintf(numero_str, "%d", cont);
                cont--;
                if(cont <= 0){
                    autentificados = true;
                }
                send(client_fd, numero_str, strlen(numero_str), 0);
            } else {
                printf("Credenciales inválidas para: %s\n", username);
                const char *response = "0";
                send(client_fd, response, strlen(response), 0);
            }
        }
        else{
            char *jugador = strtok(buffer, ":");
            char *mensaje = strtok(NULL, ":");
            printf("Jugador: %s\n", jugador);
            printf("le Toca a: %s\n", leTocaA);
            printf("Mensaje: %s\n", mensaje);
            if(strcmp(jugador, leTocaA) != 0 && strcmp(mensaje, "Me toca?") == 0){
                //printf("Envie: No, pregunto: %s\n", jugador);
                const char *response = "No";
                send(client_fd, response, strlen(response), 0);
            }
            else if(strcmp(jugador, leTocaA) == 0 && strcmp(mensaje, "Me toca?") == 0){
                char tableroStr[256];
                sprintf(tableroStr, "[[%d, %d, %d, %d, %d, %d, %d],[%d, %d, %d, %d, %d, %d, %d],[%d, %d, %d, %d, %d, %d, %d],[%d, %d, %d, %d, %d, %d, %d],[%d, %d, %d, %d, %d, %d, %d],[%d, %d, %d, %d, %d, %d, %d]]",
                        tablero[0][0], tablero[0][1], tablero[0][2], tablero[0][3], tablero[0][4], tablero[0][5], tablero[0][6],
                        tablero[1][0], tablero[1][1], tablero[1][2], tablero[1][3], tablero[1][4], tablero[1][5], tablero[1][6],
                        tablero[2][0], tablero[2][1], tablero[2][2], tablero[2][3], tablero[2][4], tablero[2][5], tablero[2][6],
                        tablero[3][0], tablero[3][1], tablero[3][2], tablero[3][3], tablero[3][4], tablero[3][5], tablero[3][6],
                        tablero[4][0], tablero[4][1], tablero[4][2], tablero[4][3], tablero[4][4], tablero[4][5], tablero[4][6],
                        tablero[5][0], tablero[5][1], tablero[5][2], tablero[5][3], tablero[5][4], tablero[5][5], tablero[5][6]);
                char response[strlen(tableroStr) + 4];
                sprintf(response, "Si:%s", tableroStr);
                printf("Envie: %s\n", response);
                send(client_fd, response, strlen(response), 0);
                printf("Si se enviooooooooooooooooooooooooooooooooooo\n");
            }
            else if(strcmp(jugador, leTocaA) == 0){
                //Lo guarda
                printf("va a guardar\n");
                /*
                char *fila;
                char *element;
                int i = 0, j;

                do {
                    fila = strtok(mensaje, "["); // Obtener la primera fila
                    printf("Siguiente fila: %s\n", fila);
                    j = 0;
                    do{
                        element = strtok(fila, ",");
                        printf("elemento: %s\n", element);
                        tableroMensaje[i][j] = stringToInt(element); // Convertir la cadena a entero y almacenarla en la matriz
                        j++;
                        element = strtok(NULL, ","); // Obtener el siguiente elemento de la fila
                    }
                    while (element != NULL); 
                    printf("%s\n",fila);
                    i++;
                    fila = strtok(NULL, "["); // Obtener la siguiente fila
                } while (fila != NULL);*/
                int it=0, fil=0, col=0;
                while(mensaje[it]!='\0'){
                    if(mensaje[it]==']'){
                        col++;
                        fil=0;
                    }
                    if(mensaje[it]>='0'&&mensaje[it]<='9'){
                        tablero[col][fil]=(int)(mensaje[it]-'0');
                        fil++;
                    }
                    it++;
                }
                // Imprimiendo el tablero
                printf("Tablero guardado: \n");
                for (int i = 0; i < 6; i++) {
                    for (int j = 0; j < 7; j++) {
                        printf("%d ", tablero[i][j]);
                    }
                    printf("\n");  // Salto de línea después de cada fila
                }
                //logica para ver si alguien ya gano
                printf("Turno cambiado\n");
                if(leTocaA == "1"){
                    leTocaA = "2";
                }
                else{
                    leTocaA = "1";
                }
            }
        }
        printf("---------------------------------------\n");
        close(client_fd);
    }

    close(server_fd);
    return 0;
}