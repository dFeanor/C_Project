#include "Picture.h"
#include <iostream>
using std::cout;


int main() {

    std::cout << "\n[ШАГ 3] Тестирование addWalls() на полученном срезе..." << std::endl;

    uint64_t oldH = testSlice.getDim1();
    uint64_t oldW = testSlice.getDim2();

    testSlice.addWalls();

    std::cout << "-> Стенки добавлены." << std::endl;
    std::cout << "-> Старые размеры: " << oldH << " x " << oldW << std::endl;
    std::cout << "-> Новые размеры:  " << testSlice.getDim1() << " x " << testSlice.getDim2() << std::endl;

    if (testSlice.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_2_slice_with_walls.raw")) {
        std::cout << "-> Файл сохранен: result_2_slice_with_walls.raw" << std::endl;
    }

    std::cout << "\n[ШАГ 4] Тестирование extractSubregion()..." << std::endl;

    Picture subRegion;

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

    std::cout << "\n[ШАГ 5] Проверка срезов по осям X и Y (сохранение файлов)..." << std::endl;

    Picture sliceX, sliceY;

    if (pic3D.extractSlice(sliceX, SliceAxis::X, pic3D.getDim2() / 2)) {
        sliceX.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_4_slice_X.raw");
        std::cout << "-> X-срез сохранен." << std::endl;
    }

    if (pic3D.extractSlice(sliceY, SliceAxis::Y, pic3D.getDim1() / 2)) {
        sliceY.saveToFile("D:\\VS_Projects\\Magister_projects\\C_Project\\images\\result_5_slice_Y.raw");
        std::cout << "-> Y-срез сохранен." << std::endl;
    }

    std::cout << "\n=== ТЕСТ ЗАВЕРШЕН УСПЕШНО ===" << std::endl;


    //проверка библиотеки стандартных изображений 
    string path = "../images/";
    Picture testImage;
    std::cout << "--- START TEST: Standard Images ---" << std::endl;
    testImage.createCylinder(200, 200, 50.0);
    testImage.addWalls();
    std::string cylinderPath = path + "gen_cylinder.raw";
    if (testImage.saveToFile(cylinderPath)) {
        std::cout << "Cylinder saved to: " << cylinderPath << std::endl;
    }
    testImage.createTortuousChannel(400, 100, 30.0, 20.0, 0.05);
    testImage.addWalls();
    std::string channelPath = path + "gen_channel.raw";
    if (testImage.saveToFile(channelPath)) {
        std::cout << "Channel saved to: " << channelPath << std::endl;
    }
    testImage.createCylinderWithPore(200, 200, 60.0, 20.0);
    testImage.addWalls();
    std::string porePath = path + "gen_cylinder_with_pore.raw";
    if (testImage.saveToFile(porePath)) {
        std::cout << "Cylinder with pore saved to: " << porePath << std::endl;
    }
    std::cout << "--- END TEST ---" << std::endl;

    return 0;
}