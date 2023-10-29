#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "ERROR 1 !\n");
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        fprintf(stderr, "ERROR 2 !\n");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); 
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "ERROR 3 !\n");
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        fprintf(stderr, "ERROR 4 !\n");
        return 1;
    }

    printf("HTTP Server Start!. Please Connect http://127.0.0.1:8080 .\n");

    while (1) {
        struct sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            fprintf(stderr, "ERROR 5 !\n");
            continue;
        }

        const char* response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(clientSocket, response_header, strlen(response_header), 0);

        FILE* file = fopen("index.html", "r");
        if (file != NULL) {
            char buffer[1024];
            size_t bytesRead;

            while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
                send(clientSocket, buffer, bytesRead, 0);
            }

            fclose(file);
        } else {
            const char* error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Not Found</h1></body></html>";
            send(clientSocket, error_response, strlen(error_response), 0);
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);

    WSACleanup();

    return 0;
}
