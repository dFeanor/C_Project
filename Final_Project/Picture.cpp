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

// Реализация метода для сохранения 2D изображения в файл
bool Picture::saveToFile(const std::string& filePath) const {
    // --- Шаг 1: Проверки ---
    if (pixels == nullptr) {
        std::cerr << "Error: Cannot save an empty picture." << std::endl;
        return false;
    }
    // Этот метод предназначен для 2D-формата, который вы описали
    if (N3 > 0) {
        std::cerr << "Error: saveToFile is implemented for 2D images only." << std::endl;
        return false;
    }

    // --- Шаг 2: Открытие файла для записи ---
    // std::ofstream - output file stream
    // std::ios::binary - обязательный флаг для бинарной записи
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
        return false;
    }

    // --- Шаг 3: Запись заголовка ---
    // Метод write принимает указатель на char, поэтому используем reinterpret_cast
    file.write(reinterpret_cast<const char*>(&N1), sizeof(N1));
    file.write(reinterpret_cast<const char*>(&N2), sizeof(N2));
    file.write(reinterpret_cast<const char*>(&resolution), sizeof(resolution));

    // --- Шаг 4: Запись пикселей ---
    uint64_t totalPixels = N1 * N2;
    file.write(reinterpret_cast<const char*>(pixels), totalPixels);

    // Проверяем, не возникло ли ошибок во время записи
    if (!file) {
        std::cerr << "Error: An error occurred while writing to file: " << filePath << std::endl;
        return false;
    }

    // Файл закроется автоматически, когда объект 'file' выйдет из области видимости

    std::cout << "Successfully saved image to " << filePath << std::endl;
    return true;
}


// Реализация метода для вырезания квадратной подобласти
bool Picture::extractSubregion(Picture& outputPicture, uint64_t startX, uint64_t startY, uint64_t size) const {
    // --- Шаг 1: Проверки исходного изображения ---
    if (pixels == nullptr) {
        std::cerr << "Error: Cannot extract subregion from an empty picture. Load a file first." << std::endl;
        return false;
    }
    if (N3 > 0) {
        std::cerr << "Error: extractSubregion is implemented for 2D images only. Current image is 3D." << std::endl;
        return false;
    }

    // --- Шаг 2: Проверки входных параметров ---
    if (size == 0) {
        std::cerr << "Error: Subregion size cannot be zero." << std::endl;
        return false;
    }
    if (startX >= N2 || startY >= N1) {
        std::cerr << "Error: Start coordinates (" << startX << ", " << startY << ") are out of bounds." << std::endl;
        return false;
    }
    if (startX + size > N2 || startY + size > N1) {
        std::cerr << "Error: Subregion (" << startX << "," << startY << ") with size " << size
            << " extends beyond image dimensions (" << N2 << ", " << N1 << ")." << std::endl;
        return false;
    }

    // --- Шаг 3: Проверка объекта outputPicture ---
    // Убедимся, что outputPicture пуст, чтобы избежать утечек памяти или перезаписи
    if (outputPicture.pixels != nullptr) {
        std::cerr << "Error: Output picture is not empty. Please provide an empty Picture object." << std::endl;
        return false;
    }

    // --- Шаг 4: Вычисление размеров новой подобласти ---
    uint64_t new_N1 = size; // Новые строки = размер квадрата
    uint64_t new_N2 = size; // Новые столбцы = размер квадрата
    uint64_t newTotalPixels = new_N1 * new_N2;

    // --- Шаг 5: Выделение памяти для новой подобласти ---
    unsigned char* newPixelsBuffer = new (std::nothrow) unsigned char[newTotalPixels];
    if (newPixelsBuffer == nullptr) {
        std::cerr << "Error: Failed to allocate memory for subregion pixels." << std::endl;
        return false;
    }

    // --- Шаг 6: Копирование данных пикселей ---
    // Итерируемся по каждой строке новой подобласти
    for (uint64_t r = 0; r < new_N1; ++r) {
        // Вычисляем индекс начального пикселя в текущей строке ИСХОДНОГО изображения
        uint64_t sourceRowStartIdx = (startY + r) * N2 + startX;
        // Вычисляем индекс начального пикселя в текущей строке НОВОГО изображения
        uint64_t destRowStartIdx = r * new_N2;

        // Копируем всю строку пикселей.
        // Количество байт для копирования равно new_N2 (размер строки в новой подобласти)
        std::memcpy(newPixelsBuffer + destRowStartIdx,
            pixels + sourceRowStartIdx,
            new_N2 * sizeof(unsigned char));
    }

    // --- Шаг 7: Заполнение полей outputPicture ---
    outputPicture.N1 = new_N1;
    outputPicture.N2 = new_N2;
    outputPicture.N3 = 0; // Всегда 0 для 2D
    outputPicture.resolution = this->resolution; // Разрешение обычно наследуется
    outputPicture.pixels = newPixelsBuffer;
    outputPicture.wallsAdded = false; // Новое изображение, стенки еще не добавлялись

    std::cout << "Successfully extracted subregion (" << startX << ", " << startY << ") with size " << size
        << ". New dimensions: " << new_N1 << "x" << new_N2 << std::endl;
    return true;
}

// Реализация метода для извлечения 2D-слайса из 3D-изображения
bool Picture::extractSlice(Picture& outputSlice, SliceAxis axis, uint64_t sliceIndex) const {
    // --- Шаг 1: Проверки ---
    if (pixels == nullptr) {
        std::cerr << "Error: Cannot extract a slice from an empty picture." << std::endl;
        return false;
    }
    if (N3 == 0) { // Проверяем, что изображение трехмерное
        std::cerr << "Error: Slices can only be extracted from 3D images." << std::endl;
        return false;
    }
    if (outputSlice.pixels != nullptr) {
        std::cerr << "Error: Output picture for the slice must be empty." << std::endl;
        return false;
    }

    uint64_t new_N1 = 0, new_N2 = 0;

    // --- Шаг 2: Валидация индекса и определение размеров слайса ---
    switch (axis) {
    case SliceAxis::X:
        if (sliceIndex >= N2) {
            std::cerr << "Error: X-axis slice index " << sliceIndex << " is out of bounds [0, " << N2 - 1 << "]." << std::endl;
            return false;
        }
        new_N1 = N1; // Высота
        new_N2 = N3; // Ширина (будет из глубины)
        break;
    case SliceAxis::Y:
        if (sliceIndex >= N1) {
            std::cerr << "Error: Y-axis slice index " << sliceIndex << " is out of bounds [0, " << N1 - 1 << "]." << std::endl;
            return false;
        }
        new_N1 = N2; // Высота (будет из ширины)
        new_N2 = N3; // Ширина (будет из глубины)
        break;
    case SliceAxis::Z:
        if (sliceIndex >= N3) {
            std::cerr << "Error: Z-axis slice index " << sliceIndex << " is out of bounds [0, " << N3 - 1 << "]." << std::endl;
            return false;
        }
        new_N1 = N1; // Высота
        new_N2 = N2; // Ширина
        break;
    }

    // --- Шаг 3: Выделение памяти ---
    uint64_t totalNewPixels = new_N1 * new_N2;
    unsigned char* newPixels = new (std::nothrow) unsigned char[totalNewPixels];
    if (newPixels == nullptr) {
        std::cerr << "Error: Failed to allocate memory for the slice." << std::endl;
        return false;
    }

    // --- Шаг 4: Копирование пикселей ---
    // Данные в 3D хранятся так: [сначала все пиксели для z=0, потом для z=1, ...]
    // Индекс пикселя (i, j, k) -> k*(N1*N2) + i*N2 + j
    // где i - строка (Y), j - столбец (X), k - глубина (Z)

    uint64_t currentNewPixel = 0;
    switch (axis) {
    case SliceAxis::Z: { // Самый простой случай: данные лежат подряд
        uint64_t offset = sliceIndex * (N1 * N2);
        std::memcpy(newPixels, pixels + offset, totalNewPixels);
        break;
    }
    case SliceAxis::Y: { // Фиксируем строку 'i', итерируем по 'j' и 'k'
        for (uint64_t k = 0; k < N3; ++k) { // Итерация по глубине
            for (uint64_t j = 0; j < N2; ++j) { // Итерация по ширине
                uint64_t sourceIndex = k * (N1 * N2) + sliceIndex * N2 + j;
                newPixels[currentNewPixel++] = pixels[sourceIndex];
            }
        }
        break;
    }
    case SliceAxis::X: { // Фиксируем столбец 'j', итерируем по 'i' и 'k'
        for (uint64_t k = 0; k < N3; ++k) { // Итерация по глубине
            for (uint64_t i = 0; i < N1; ++i) { // Итерация по высоте
                uint64_t sourceIndex = k * (N1 * N2) + i * N2 + sliceIndex;
                newPixels[currentNewPixel++] = pixels[sourceIndex];
            }
        }
        break;
    }
    }

    // --- Шаг 5: Заполнение полей выходного объекта ---
    outputSlice.N1 = new_N1;
    outputSlice.N2 = new_N2;
    outputSlice.N3 = 0; // Слайс - это 2D изображение
    outputSlice.resolution = this->resolution;
    outputSlice.pixels = newPixels;
    outputSlice.wallsAdded = false;

    std::cout << "Successfully extracted slice." << std::endl;
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
