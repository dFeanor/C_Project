#include "Picture.h"
#include <iostream>
using std::cout;


int main() {

    std::string filePath = "C:/QtProjects/C_Project-main/images/picture_3d.raw";
    std::cout << "START TEST" << std::endl;

    Picture pic3D;
    std::cout << "[STEP 1] 3D Loading: " << filePath << std::endl;

    if (!pic3D.loadFromFile(filePath, true)) {
        std::cerr << "Error: Can't open file: " << filePath << std::endl;
        return 1;
    }

    std::cout << "-> SUCCESS. 3D: "
        << pic3D.getDim1() << " x "
        << pic3D.getDim2() << " x "
        << pic3D.getDim3() << std::endl;

    Picture testSlice;
    uint64_t sliceIndex = pic3D.getDim3() / 2;

    std::cout << "[STEP 2] Z-dim (index " << sliceIndex << ")..." << std::endl;

    if (pic3D.extractSlice(testSlice, SliceAxis::Z, sliceIndex)) {
        std::cout << "-> SUCCESS. 2D picture." << std::endl;
        std::cout << "-> Slice sizes: " << testSlice.getDim1() << " x " << testSlice.getDim2() << std::endl;

        // Сохраним "чистый" срез для контроля
        testSlice.saveToFile("C:/QtProjects/C_Project-main/images/result_1_raw_slice.raw");
    }
    else {
        std::cerr << "Error: Can't extract slice" << std::endl;
        return 1;
    }

    std::cout << "[STEP 3] addWalls() test on slice" << std::endl;

    // Запоминаем старые размеры для проверки
    uint64_t oldH = testSlice.getDim1();
    uint64_t oldW = testSlice.getDim2();

    testSlice.addWalls();

    std::cout << "-> Walls added." << std::endl;
    std::cout << "-> Old size: " << oldH << " x " << oldW << std::endl;
    std::cout << "-> New size:  " << testSlice.getDim1() << " x " << testSlice.getDim2() << std::endl;

    // Сохраняем результат
    if (testSlice.saveToFile("C:/QtProjects/C_Project-main/images/result_2_slice_with_walls.raw")) {
        std::cout << "-> File saved: result_2_slice_with_walls.raw" << std::endl;
    }

    std::cout << "[STEP 4] Test of extractSubregion()..." << std::endl;

    Picture subRegion;

    // Попробуем вырезать квадрат из центра изображения
    // Размеры берем динамически, чтобы не выйти за границы
    uint64_t size = std::min(testSlice.getDim1(), testSlice.getDim2()) / 4;
    uint64_t startX = testSlice.getDim2() / 2 - size / 2;
    uint64_t startY = testSlice.getDim1() / 2 - size / 2;

    std::cout << "-> Try to extraxt square " << size << "x" << size
        << " in coords (" << startX << ", " << startY << ")" << std::endl;

    if (testSlice.extractSubregion(subRegion, startX, startY, size)) {
        std::cout << "-> SUCCESS. Size of subregion: "
            << subRegion.getDim1() << " x " << subRegion.getDim2() << std::endl;

        // Сохраняем результат
        if (subRegion.saveToFile("C:/QtProjects/C_Project-main/images/result_3_subregion.raw")) {
            std::cout << "-> File saved: result_3_subregion.raw" << std::endl;
        }
    }
    else {
        std::cerr << "ERROR: Can't extract subregion" << std::endl;
    }

    std::cout << "\n[Step 5] X and Y slices..." << std::endl;

    Picture sliceX, sliceY;

    // X-срез (середина)
    if (pic3D.extractSlice(sliceX, SliceAxis::X, pic3D.getDim2() / 2)) {
        sliceX.saveToFile("C:/QtProjects/C_Project-main/images/result_4_slice_X.raw");
        std::cout << "-> X-slice saved." << std::endl;
    }

    // Y-срез (середина)
    if (pic3D.extractSlice(sliceY, SliceAxis::Y, pic3D.getDim1() / 2)) {
        sliceY.saveToFile("C:/QtProjects/C_Project-main/images/result_5_slice_Y.raw");
        std::cout << "-> Y-slice saved." << std::endl;
    }

    //проверка библиотеки стандартных изображений 
    string path = "C:/QtProjects/C_Project-main/images/";
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

    Picture poreImage;
    testImage.loadFromFile(porePath, false);
    testImage.removeIsolatedPores();
    std::string noporePath = path + "gen_cylinder_without_pore.raw";
    if (testImage.saveToFile(noporePath)) {
        std::cout << "Cylinder without pore saved to: " << porePath << std::endl;
    }

    std::cout << "--- END TEST ---" << std::endl;

    return 0;
}
