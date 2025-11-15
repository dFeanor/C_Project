#ifndef PICTURE_H
#define PICTURE_H

#include <string>
#include <vector>
#include <cstdint> // <--- ВКЛЮЧАЕМ ДЛЯ ТИПОВ С ФИКСИРОВАННЫМ РАЗМЕРОМ

class Picture {
public:
    Picture();
    ~Picture();

    bool loadFromFile(const std::string& filePath, bool is3D = false);

    //uint64_t — это 64-битный (строго 8 байт) беззнаковый целочисленный тип данных в C++, который хранит целые числа
    uint64_t getDim1() const;
    uint64_t getDim2() const;
    uint64_t getDim3() const;

    double getResolution() const;
    uint64_t getTotalPixels() const;
    const unsigned char* getPixelData() const;

    // Метод для добавления стенок
    void addWalls();

    //Вспомогательный метод для проверок и прочего
    void PrintPicture();

private:
    // Используем типы с гарантированным размером 8 байт
    uint64_t N1;
    uint64_t N2;
    uint64_t N3;

    double resolution;
    unsigned char* pixels;

    // Флаг, чтобы отслеживать, были ли уже добавлены стенки
    bool wallsAdded;

    Picture(const Picture&);
    Picture& operator=(const Picture&);
};

#endif // PICTURE_H
