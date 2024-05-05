#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

int main() {
    // Создание сокета
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Ошибка при создании сокета\n";
        return -1;
    }

    // Установка параметров адреса сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Принимаем соединения на всех интерфейсах
    serverAddr.sin_port = htons(8888); // Порт сервера

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка при привязке сокета\n";
        return -1;
    }

    // Начало прослушивания порта
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Ошибка при прослушивании порта\n";
        return -1;
    }

    std::cout << "Сервер запущен. Ожидание подключений...\n";

    // Принятие входящих соединений
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << "Ошибка при принятии соединения\n";
        return -1;
    }

    std::cout << "Клиент подключен\n";

    // Отправка сообщения клиенту
    const char *message = "Привет от сервера!";
    send(clientSocket, message, strlen(message), 0);

    // Закрытие сокетов
    close(clientSocket);
    close(serverSocket);

    return 0;
}
