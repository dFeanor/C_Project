#include "Picture.h"
#include <iostream>

int main() {
    Picture originalImage;

    // 1. Загружаем исходное изображение
    if (!originalImage.loadFromFile("picture_2d.raw", false)) {
        return 1;
    }

    std::cout << "\n--- Original Image Info ---" << std::endl;
    originalImage.PrintPicture();

    // 2. Создаем пустой объект для хранения подобласти
    Picture subregionImage;

    // 3. Вырезаем подобласть:
    //    Предположим, мы хотим вырезать квадрат 50x50 пикселей,
    //    начиная с координат (10, 20)
    uint64_t startX = 10;
    uint64_t startY = 20;
    uint64_t size = 50;

    if (originalImage.extractSubregion(subregionImage, startX, startY, size)) {
        std::cout << "\n--- Extracted Subregion Info ---" << std::endl;
        subregionImage.PrintPicture();

        // 4. Сохраняем вырезанную подобласть в новый файл
        std::string subregionFilename = "subregion_" + std::to_string(startX) + "_" + std::to_string(startY) + "_s" + std::to_string(size) + ".raw";
        if (subregionImage.saveToFile(subregionFilename)) {
            std::cout << "Subregion saved to " << subregionFilename << std::endl;
        }
        else {
            std::cerr << "Failed to save subregion." << std::endl;
        }
    }
    else {
        std::cerr << "Failed to extract subregion." << std::endl;
    }

    // Пример ошибки: попытка вырезать за пределами изображения
    std::cout << "\n--- Attempting to extract an invalid subregion ---" << std::endl;
    Picture invalidSubregion;
    originalImage.extractSubregion(invalidSubregion, originalImage.getDim2() - 10, originalImage.getDim1() - 10, 20); // Выходит за границы

    return 0;
}
