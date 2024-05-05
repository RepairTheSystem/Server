#include <iostream>
#include <vector>
#include <thread>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Структура для хранения информации об игроке
struct Player {
    int level;
    int health;
    bool active; // Флаг, показывающий, активен ли игрок
};

// Функция для обработки перемещения игрока
void movePlayer(Player &player) {
    // Генерируем случайное число для определения события
    int event = rand() % 2; // 0 - монстр, 1 - сокровище

    if (event == 0) {
        // Генерируем уровень монстра
        int monsterLevel = rand() % 10 + 1; // Уровень монстра от 1 до 10

        std::cout << "Появился монстр уровня " << monsterLevel << "!\n";

        // Предлагаем игроку сразиться или убежать
        std::cout << "Выберите действие:\n";
        std::cout << "1. Сразиться\n";
        std::cout << "2. Убежать\n";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            // Игрок выбрал сражение
            if (player.level > monsterLevel) {
                std::cout << "Вы победили монстра!\n";
                // Увеличиваем уровень игрока на 2
                player.level += 2;
            } else {
                std::cout << "Монстр слишком силен, вы проиграли!\n";
                // Игрок проиграл, можно добавить дополнительную логику здесь
            }
        }
        else {
            // Игрок выбрал убегать
            std::cout << "Вы убежали от монстра!\n";
        }
    } 
    else {
        std::cout << "Появились сокровища!\n";

        // Предлагаем игроку забрать их или уйти
        std::cout << "Выберите действие:\n";
        std::cout << "1. Забрать\n";
        std::cout << "2. Уйти\n";

        // Далее можно добавить логику получения сокровищ
        int chose;
        std::cin >> chose;
        if (chose == 1){
            int trapChance = rand() % 10;
            if (trapChance == 1) {
                std::cout << "В сокровищах оказалась ловушка! Вы теряете 1 уровень\n";
                player.level -= 1;
            }
            else {
                std::cout << "Вы собираете сокровища!\n";
                player.level += 1;
            }
        }
        else if (chose == 2){
            std::cout << "Вы решаете не рисковать \n";
        }
        else {
            std::cout << "Неизвестная команда\n";
        }
    }
}

// Функция для обработки запросов от клиента
void handleClient(int clientSocket, Player &player) {
    char buffer[1024] = {0};

    while (true) {
        // Принимаем сообщение от клиента
        int valread = recv(clientSocket, buffer, 1024, 0);
        if (valread < 0) {
            std::cerr << "Ошибка при чтении из сокета\n";
            close(clientSocket);
            return;
        }

        // Выводим полученное сообщение от клиента
        std::cout << "Получено от клиента: " << buffer << std::endl;

        // Обработка запроса от клиента
        if (strcmp(buffer, "view_stats") == 0) {
            // Отправляем информацию о характеристиках игрока
            std::string response = "Level: " + std::to_string(player.level) + ", Health: " + std::to_string(player.health);
            send(clientSocket, response.c_str(), response.length(), 0);
        } else if (strcmp(buffer, "move") == 0) {
            movePlayer(player);
            // Здесь можно вызвать функцию movePlayer или добавить свою логику
            std::string response = "Player moved!";
            send(clientSocket, response.c_str(), response.length(), 0);
        } else if (strcmp(buffer, "exit") == 0) {
            // Клиент запросил завершение соединения
            break;
        } else {
            // Неизвестная команда
            std::string response = "Unknown command";
            send(clientSocket, response.c_str(), response.length(), 0);
        }

        // Очищаем буфер
        memset(buffer, 0, sizeof(buffer));
    }

    // Закрываем сокет клиента
    close(clientSocket);
}


int main() {
    // Создаем игрока
    Player player;
    player.level = 1;
    player.health = 100;
    player.active = true;

    // Создаем серверный сокет
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Ошибка при создании сокета\n";
        return -1;
    }

    // Устанавливаем параметры адреса сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);

    // Привязываем сокет к адресу и порту
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Ошибка при привязке сокета\n";
        return -1;
    }

    // Начинаем прослушивать порт
    if (listen(serverSocket, 5) < 0) {
        std::cerr << "Ошибка при прослушивании порта\n";
        return -1;
    }

    std::cout << "Сервер запущен. Ожидание подключений...\n";

    while (true) {
        // Принимаем подключение от клиента
        int clientSocket;
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Ошибка при принятии соединения\n";
            continue;
        }

        // Создаем поток для обработки запроса клиента
        std::thread clientThread(handleClient, clientSocket, std::ref(player));
        clientThread.detach(); // Отсоединяем поток, чтобы не ждать его завершения
    }

    // Закрываем сокет сервера
    close(serverSocket);

    return 0;
}