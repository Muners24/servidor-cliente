#define RAYLIB_NO_WINDOWS_CONFLICTS
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "C:\raylib\raylib\src\raylib.h"

#pragma comment(lib, "Ws2_32.lib")

const std::string SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 12345;
using std::string;

int main() {
    // Inicializar Winsock
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        return 1;
    }

    // Crear el socket TCP
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

    // Conectar al servidor
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al conectar al servidor" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Inicializar Raylib
    InitWindow(800, 600, "Raylib y Winsock Ejemplo");
    SetTargetFPS(60);
    bool key;
    string message;
    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Hello, Raylib and Winsock!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();

        // Detectar input y enviar al servidor
        key = false;
        message = "";
        if (IsKeyDown(KEY_W)) {
            message = "W key pressed";
            key = true;
        }
        else if (IsKeyDown(KEY_S)) {
            message = "S key pressed";
            key = true;
        }
        // Enviar el mensaje si no está vacío
        if(key)
        {
            send(clientSocket, message.c_str(), message.length(),0);

            char buffer[512];
            int bytesReceived = recv(clientSocket, buffer, 512, 0);
            if (bytesReceived > 0)
            {
                std::cout << std::string(buffer, 0, bytesReceived) << std::endl;
            }
        }
    }

    // Cerrar la conexión y limpiar
    closesocket(clientSocket);
    WSACleanup();
    CloseWindow();
    return 0;
}
