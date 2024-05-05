#include <iostream>
#include <fstream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

// Функция для обработки изображения
vector<vector<int>> processImage(const vector<vector<int>>& inputImage) {
    // Здесь можно добавить код для обработки изображения, например, поворот или размытие
    // В этом примере просто возвращаем исходное изображение
    return inputImage;
}

// Функция для загрузки изображения из файла BMP
vector<vector<int>> loadImage(const string& filename) {
    vector<vector<int>> image;

    // Открываем файл BMP для чтения
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not open file " << filename << endl;
        return image;
    }

    // Пропускаем заголовок BMP (54 байта)
    file.seekg(54);

    // Считываем пиксели изображения
    int width, height;
    file.read(reinterpret_cast<char*>(&width), 4);
    file.read(reinterpret_cast<char*>(&height), 4);

    // Читаем пиксели изображения
    for (int y = 0; y < height; ++y) {
        vector<int> row;
        for (int x = 0; x < width; ++x) {
            int pixel;
            file.read(reinterpret_cast<char*>(&pixel), 3); // Считываем RGB компоненты пикселя
            row.push_back(pixel);
        }
        image.push_back(row);
    }

    file.close();
    return image;
}

// Функция для сохранения изображения в файл BMP
void saveImage(const vector<vector<int>>& image, const string& filename) {
    // Создаем файл BMP и записываем в него изображение
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error: Could not create file " << filename << endl;
        return;
    }

    // Заголовок BMP (54 байта)
    const int width = image[0].size();
    const int height = image.size();
    const int imageSize = width * height * 3; // Размер изображения в байтах
    const int fileSize = imageSize + 54; // Размер файла в байтах

    // Заголовок файла BMP
    file << "BM"; // Сигнатура BMP
    file.write(reinterpret_cast<const char*>(&fileSize), 4); // Размер файла
    file.write(reinterpret_cast<const char*>(&width), 4); // Ширина изображения
    file.write(reinterpret_cast<const char*>(&height), 4); // Высота изображения
    file.write("\0\0\0\0", 4); // Зарезервированные байты
    file.write("\x36\0\0\0", 4); // Смещение до начала пиксельных данных (54 байта)

    // Пиксельные данные (RGB компоненты)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int& pixel = image[y][x];
            file.write(reinterpret_cast<const char*>(&pixel), 3);
        }
    }

    file.close();
}

int main() {
    // Создаем сокет
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Error: Could not create socket" << endl;
        return 1;
    }

    // Настраиваем адрес сервера
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    // Привязываем сокет к адресу сервера
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Error: Could not bind socket to address" << endl;
        close(serverSocket);
        return 1;
    }

    // Начинаем прослушивать входящие соединения
    if (listen(serverSocket, 10) == -1) {
        cerr << "Error: Could not listen on socket" << endl;
        close(serverSocket);
        return 1;
    }

    cout << "Server started. Waiting for connections..." << endl;

    while (true) {
        // Принимаем входящее соединение
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == -1) {
            cerr << "Error: Could not accept connection" << endl;
            continue;
        }

        // Получаем данные от клиента
        vector<unsigned char> buffer(4096);
        int bytesReceived = recv(clientSocket, buffer.data(), buffer.size(), 0);
        if (bytesReceived == -1) {
            cerr << "Error: Could not receive data from client" << endl;
            close(clientSocket);
            continue;
        }

        // Процесс обработки изображения и отправки обратно клиенту
        // В данном примере мы просто пересылаем полученные данные обратно клиенту
        send(clientSocket, buffer.data(), bytesReceived, 0);

        cout << "Image processed and sent back to the client." << endl;

        // Закрываем соединение с клиентом
        close(clientSocket);
    }

    // Закрываем серверный сокет
    close(serverSocket);

    return 0;
}
