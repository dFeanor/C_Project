#include <iostream>
#include <string>
#include "Picture.h"

int main() {
    // Настройка локали для вывода русского текста
    setlocale(LC_ALL, "");

    // Путь к вашему файлу (с двойными слэшами для Windows)
    std::string filePath = "D:\\VS_Projects\\Magister_projects\\C_Project\\images\\picture_3d.raw";

    std::cout << "=== ЗАПУСК ИНТЕГРАЦИОННОГО ТЕСТА ===" << std::endl;

    // ---------------------------------------------------------
    // 1. Загрузка исходного 3D изображения
    // ---------------------------------------------------------
    Picture pic3D;
    std::cout << "\n[ШАГ 1] Загрузка 3D файла: " << filePath << std::endl;

    if (!pic3D.loadFromFile(filePath, true)) {
        std::cerr << "ОШИБКА: Не удалось открыть файл. Проверьте путь!" << std::endl;
        return 1;
    }

    std::cout << "-> Успешно. Размеры 3D: "
        << pic3D.getDim1() << " x "
        << pic3D.getDim2() << " x "
        << pic3D.getDim3() << std::endl;

    // ---------------------------------------------------------
    // 2. Получение 2D изображения (Среза) из 3D
    // ---------------------------------------------------------
    Picture testSlice;
    // Берем срез посередине глубины (ось Z)
    uint64_t sliceIndex = pic3D.getDim3() / 2;

    std::cout << "\n[ШАГ 2] Извлечение Z-среза (индекс " << sliceIndex << ")..." << std::endl;

    if (pic3D.extractSlice(testSlice, SliceAxis::Z, sliceIndex)) {
        std::cout << "-> Успешно. Получено 2D изображение." << std::endl;
        std::cout << "-> Размеры среза: " << testSlice.getDim1() << " x " << testSlice.getDim2() << std::endl;

        // Сохраним "чистый" срез для контроля
        testSlice.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_1_raw_slice.raw");
    }
    else {
        std::cerr << "ОШИБКА: Не удалось извлечь срез." << std::endl;
        return 1;
    }

    // ---------------------------------------------------------
    // 3. Тестирование добавления стенок (addWalls) на срезе
    // ---------------------------------------------------------
    std::cout << "\n[ШАГ 3] Тестирование addWalls() на полученном срезе..." << std::endl;

    // Запоминаем старые размеры для проверки
    uint64_t oldH = testSlice.getDim1();
    uint64_t oldW = testSlice.getDim2();

    testSlice.addWalls();

    std::cout << "-> Стенки добавлены." << std::endl;
    std::cout << "-> Старые размеры: " << oldH << " x " << oldW << std::endl;
    std::cout << "-> Новые размеры:  " << testSlice.getDim1() << " x " << testSlice.getDim2() << std::endl;

    // Сохраняем результат
    if (testSlice.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_2_slice_with_walls.raw")) {
        std::cout << "-> Файл сохранен: result_2_slice_with_walls.raw" << std::endl;
    }

    // ---------------------------------------------------------
    // 4. Тестирование вырезания подобласти (extractSubregion)
    //    из изображения со стенками
    // ---------------------------------------------------------
    std::cout << "\n[ШАГ 4] Тестирование extractSubregion()..." << std::endl;

    Picture subRegion;

    // Попробуем вырезать квадрат из центра изображения
    // Размеры берем динамически, чтобы не выйти за границы
    uint64_t size = std::min(testSlice.getDim1(), testSlice.getDim2()) / 4;
    uint64_t startX = testSlice.getDim2() / 2 - size / 2;
    uint64_t startY = testSlice.getDim1() / 2 - size / 2;

    std::cout << "-> Попытка вырезать квадрат " << size << "x" << size
        << " в координатах (" << startX << ", " << startY << ")" << std::endl;

    if (testSlice.extractSubregion(subRegion, startX, startY, size)) {
        std::cout << "-> Успешно. Размеры подобласти: "
            << subRegion.getDim1() << " x " << subRegion.getDim2() << std::endl;

        // Сохраняем результат
        if (subRegion.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_3_subregion.raw")) {
            std::cout << "-> Файл сохранен: result_3_subregion.raw" << std::endl;
        }
    }
    else {
        std::cerr << "ОШИБКА: Не удалось вырезать подобласть." << std::endl;
    }

    // ---------------------------------------------------------
    // 5. Дополнительная проверка: срезы по другим осям
    // ---------------------------------------------------------
    std::cout << "\n[ШАГ 5] Проверка срезов по осям X и Y (сохранение файлов)..." << std::endl;

    Picture sliceX, sliceY;

    // X-срез (середина)
    if (pic3D.extractSlice(sliceX, SliceAxis::X, pic3D.getDim2() / 2)) {
        sliceX.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_4_slice_X.raw");
        std::cout << "-> X-срез сохранен." << std::endl;
    }

    // Y-срез (середина)
    if (pic3D.extractSlice(sliceY, SliceAxis::Y, pic3D.getDim1() / 2)) {
        sliceY.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_5_slice_Y.raw");
        std::cout << "-> Y-срез сохранен." << std::endl;
    }

    std::cout << "\n=== ТЕСТ ЗАВЕРШЕН УСПЕШНО ===" << std::endl;

    return 0;
}