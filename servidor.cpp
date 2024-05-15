#define RAYLIB_NO_WINDOWS_CONFLICTS
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <list>
#include <thread>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

const int SERVER_PORT = 12345;

using std::thread;
using std::list;

void HandleClient(SOCKET clientSocket,int numeroCliente)
{
    while (true)
    {
        char buffer[512];
        int bytesReceived = recv(clientSocket, buffer, 512, 0);
        if (bytesReceived > 0)
        {
            std::cout << "Mensaje recibido" << numeroCliente <<":" << std::string(buffer, 0, bytesReceived) << std::endl;

            // Aquí puedes procesar el mensaje recibido según tus necesidades

            // Opcionalmente, enviar una respuesta al cliente
            const char *responseMessage = "Mensaje recibido correctamente.";
            send(clientSocket, responseMessage, strlen(responseMessage), 0);
        }
        else if (bytesReceived == 0)
        {
            // El cliente cerró la conexión
            std::cout << "Cliente desconectado." << std::endl;
            break; // Salir del bucle
        }
        else
        {
            // Ocurrió un error al recibir el mensaje
            std::cerr << "Error al recibir el mensaje del cliente" << std::endl;
            break; // Salir del bucle
        }
    }

    // Cerrar la conexión
    closesocket(clientSocket);
}

int main()
{
    // Inicializar Winsock
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
    {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        return 1;
    }

    // Crear el socket TCP
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == INVALID_SOCKET)
    {
        std::cerr << "Error al crear el socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurar la dirección del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Enlazar el socket a la dirección y puerto
    if (bind(listeningSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Error al enlazar el socket" << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    // Poner el socket en modo de escucha
    if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Error al poner el socket en modo de escucha" << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    list<SOCKET> clientSokets;
    auto cliente = clientSokets.end();

    list<thread> clientThreads;

    int i = 0;
    while (true)
    {
        // Aceptar una conexión entrante
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(listeningSocket, reinterpret_cast<sockaddr *>(&clientAddr), &clientAddrSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Error al aceptar la conexión" << std::endl;
            closesocket(listeningSocket);
            WSACleanup();
            return 1;
        }

        // Crear un hilo para manejar al cliente
        clientThreads.emplace_back(HandleClient, clientSocket,++i);
    }

    // Cerrar la conexión y limpiar
    cliente = clientSokets.begin();
    while (cliente != clientSokets.end())
    {
        closesocket((*cliente));
        cliente++;
    }
    closesocket(listeningSocket);
    WSACleanup();
    return 0;
}
