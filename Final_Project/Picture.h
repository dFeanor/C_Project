#ifndef PICTURE_H
#define PICTURE_H

#include <string>
#include <vector>
#include <cstdint> // <--- ВКЛЮЧАЕМ ДЛЯ ТИПОВ С ФИКСИРОВАННЫМ РАЗМЕРОМ

// Определяем enum для указания оси среза. Это гораздо безопаснее, чем использовать
// просто числа (0, 1, 2), так как защищает от передачи неверных значений.
enum class SliceAxis {
    // Срез перпендикулярно оси X (оси столбцов).
    // Итоговое 2D изображение будет иметь размеры N1 (высота) x N3 (глубина).
    X,
    // Срез перпендикулярно оси Y (оси строк).
    // Итоговое 2D изображение будет иметь размеры N2 (ширина) x N3 (глубина).
    Y,
    // Срез перпендикулярно оси Z (оси глубины).
    // Итоговое 2D изображение будет иметь размеры N1 (высота) x N2 (ширина).
    Z
};


class Picture {
public:
    Picture();
    ~Picture();

    bool loadFromFile(const std::string& filePath, bool is3D = false);

    bool saveToFile(const std::string& filePath) const;

    // Принимает пустой объект Picture по ссылке для заполнения,
    // координаты верхнего левого угла (startX, startY) и размер стороны квадрата (size).
    bool extractSubregion(Picture& outputPicture, uint64_t startX, uint64_t startY, uint64_t size) const;
    //Mетод для извлечения 2D - слайса из 3D - изображения
    bool extractSlice(Picture& outputSlice, SliceAxis axis, uint64_t sliceIndex) const;

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
