#include "Picture.h"
#include <fstream>
#include <iostream>

// Реализация конструктора
Picture::Picture() : N1(0), N2(0), N3(0), resolution(0.0), pixels(nullptr) {}

// Реализация деструктора
Picture::~Picture() {
    delete[] pixels; // Освобождаем память, если она была выделена
}

// Реализация метода загрузки из файла
bool Picture::loadFromFile(const std::string& filePath, bool is3D) {
    // Открываем файл в бинарном режиме для чтения
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }

    // Читаем размеры
    file.read(reinterpret_cast<char*>(&N1), sizeof(N1));
    file.read(reinterpret_cast<char*>(&N2), sizeof(N2));

    if (is3D) {
        file.read(reinterpret_cast<char*>(&N3), sizeof(N3));
    }
    else {
        N3 = 0; // Для 2D случая третий размер равен 0
    }

    // Читаем разрешение
    file.read(reinterpret_cast<char*>(&resolution), sizeof(resolution));

    // Проверяем, удалось ли чтение заголовка
    if (!file) {
        std::cerr << "Error: Failed to read header from file " << filePath << std::endl;
        return false;
    }

    // Сбрасываем флаг при новой загрузке
    wallsAdded = false;

    // Вычисляем общее количество пикселей
    size_t totalPixels = is3D ? (N1 * N2 * N3) : (N1 * N2);
    if (totalPixels == 0) {
        std::cerr << "Error: Image dimensions are zero." << std::endl;
        return false;
    }

    // Освобождаем старую память, если она была выделена
    delete[] pixels;

    // Выделяем новую память под пиксели
    pixels = new unsigned char[totalPixels];

    // Читаем данные пикселей в выделенную память
    file.read(reinterpret_cast<char*>(pixels), totalPixels);

    if (!file) {
        std::cerr << "Error: Failed to read pixel data from file " << filePath << std::endl;
        delete[] pixels; // Очищаем память в случае ошибки
        pixels = nullptr;
        return false;
    }

    std::cout << "Successfully loaded image " << filePath << std::endl;
    return true;
}

void Picture::addWalls() {
    // --- Шаг 1: Проверки ---
    if (pixels == nullptr) {
        std::cerr << "Warning: Cannot add walls to an empty picture. Load a file first." << std::endl;
        return;
    }
    if (wallsAdded) {
        std::cout << "Info: Walls have already been added to this picture." << std::endl;
        return;
    }
    // Проверяем, что работаем с 2D изображением
    if (N3 > 0) {
        std::cerr << "Warning: The 'addTopBottomWalls' method is implemented for 2D images only." << std::endl;
        return;
    }

    std::cout << "Adding top and bottom walls..." << std::endl;

    // Определим значение пикселя для стенки (255 - порода)
    const unsigned char WALL_PIXEL_VALUE = 255;

    // --- Шаг 2: Вычисление новых размеров ---
    uint64_t old_N1 = N1;       // Старое количество строк
    uint64_t new_N1 = N1 + 2;   // Новое количество строк (старое + 2 стенки)

    // N2 (количество столбцов) не меняется
    uint64_t rowSizeBytes = N2; // Размер одной строки в байтах
    uint64_t oldTotalPixels = old_N1 * N2;
    uint64_t newTotalPixels = new_N1 * N2;

    // --- Шаг 3: Выделение новой памяти ---
    unsigned char* newPixels = new unsigned char[newTotalPixels];

    // --- Шаг 4: Заполнение нового массива ---

    // 4.1. Добавляем ВЕРХНЮЮ стенку (заполняем первую строку значением WALL_PIXEL_VALUE)
    std::memset(newPixels, WALL_PIXEL_VALUE, rowSizeBytes);

    // 4.2. Копируем старые данные изображения сразу после верхней стенки
    // Адрес назначения: начало нового массива + смещение на одну строку
    std::memcpy(newPixels + rowSizeBytes, pixels, oldTotalPixels);

    // 4.3. Добавляем НИЖНЮЮ стенку
    // Адрес назначения: начало нового массива + смещение на (1 + old_N1) строк
    std::memset(newPixels + rowSizeBytes + oldTotalPixels, WALL_PIXEL_VALUE, rowSizeBytes);

    // --- Шаг 5: Освобождение старой памяти ---
    delete[] pixels;

    // --- Шаг 6: Обновление состояния объекта ---
    pixels = newPixels; // Теперь указатель хранит адрес нового, большого массива
    N1 = new_N1;        // Обновляем количество строк
    wallsAdded = true;  // Устанавливаем флаг

    std::cout << "Successfully added walls. New dimensions (N1 x N2): " << N1 << " x " << N2 << std::endl;
}


// Реализация геттеров
size_t Picture::getDim1() const {
    return N1;
}

size_t Picture::getDim2() const {
    return N2;
}

size_t Picture::getDim3() const {
    return N3;
}

double Picture::getResolution() const {
    return resolution;
}

size_t Picture::getTotalPixels() const {
    return N3 > 0 ? (N1 * N2 * N3) : (N1 * N2);
}

const unsigned char* Picture::getPixelData() const {
    return pixels;
}

void Picture::PrintPicture() {
    std::cout << "--- Picture Info ---" << std::endl;

    // Проверяем, было ли изображение загружено (выделена ли память под пиксели)
    if (pixels == nullptr) {
        std::cout << "Picture is empty or not loaded." << std::endl;
        std::cout << "--------------------" << std::endl;
        return; // Выходим из метода, так как выводить нечего
    }

    // Определяем, является ли изображение 2D или 3D
    bool is3D = (N3 > 0);

    if (is3D) {
        std::cout << "Type: 3D" << std::endl;
        std::cout << "Dimensions (N1 x N2 x N3): " << getDim1() << " x " << getDim2() << " x " << getDim3() << std::endl;
    }
    else {
        std::cout << "Type: 2D" << std::endl;
        std::cout << "Dimensions (N1 x N2): " << getDim1() << " x " << getDim2() << std::endl;
    }

    std::cout << "Resolution: " << getResolution() << std::endl;

    uint64_t totalPixels = getTotalPixels();
    std::cout << "Total Pixels: " << totalPixels << std::endl;

    // Выведем несколько первых пикселей для примера, чтобы не загромождать консоль
    // Например, выведем не более 16 первых пикселей
    std::cout << "Sample of first pixels: ";
    if (totalPixels > 0) {
        // Определяем, сколько пикселей показать, чтобы не выйти за пределы массива
        uint64_t pixelsToShow = std::min(totalPixels, static_cast<uint64_t>(16));
        for (uint64_t i = 0; i < pixelsToShow; ++i) {
            // Кастуем к int, чтобы cout вывел число, а не символ ASCII
            std::cout << static_cast<int>(pixels[i]) << " ";
        }
    }
    else {
        std::cout << "No pixel data.";
    }

    std::cout << std::endl;
    std::cout << "--------------------" << std::endl;
}
