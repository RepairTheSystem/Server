#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Создаем клиентский сокет
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Ошибка при создании сокета\n";
        return -1;
    }

    // Устанавливаем параметры адреса сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Подключаемся к серверу
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка при подключении к серверу\n";
        return -1;
    }

    std::cout << "Подключение к серверу выполнено\n";

    // Переменная для хранения введенной команды
    std::string command;

    // Ожидаем ввода команды от пользователя
    while (true) {
        std::cout << "Введите команду ('exit' для выхода): ";
        std::cin >> command;

        // Отправляем запрос на сервер только при вводе команды "view_stats" или "move"
        if (command == "view_stats" || command == "move") {
            send(clientSocket, command.c_str(), command.length(), 0);

            // Получаем и выводим ответ от сервера
            char buffer[1024] = {0};
            recv(clientSocket, buffer, 1024, 0);
            std::cout << "Ответ от сервера: " << buffer << std::endl;

            // Если введена команда "exit", завершаем цикл
            if (command == "exit") {
                break;
            }
        } else {
            std::cout << "Некорректная команда\n";
        }
    }

    // Закрываем сокет
    close(clientSocket);

    return 0;
}
