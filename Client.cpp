#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Создание сокета
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Ошибка при создании сокета\n";
        return -1;
    }

    // Установка параметров адреса сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); // Порт сервера
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP адрес сервера

    // Подключение к серверу
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка при подключении к серверу\n";
        return -1;
    }

    std::cout << "Подключение к серверу выполнено\n";

    // Получение сообщения от сервера
    char buffer[1024] = {0};
    recv(clientSocket, buffer, 1024, 0);
    std::cout << "Сообщение от сервера: " << buffer << std::endl;

    // Закрытие сокета
    close(clientSocket);

    return 0;
}
