#include "Picture.h"
#include <iostream>
using std::cout;


int main() {

    // Picture image2D;
    // std::string path = "D:\\VS_Projects\\Magister_projects\\C_Project\\images\\";
    // Предполагается, что у вас есть 3D файл "picture_3d.raw"
    // if (!image2D.loadFromFile(path + "picture_2d.raw", true)) {
    //     return 1;
    // }


    //std::cout << "\n--- Original 3D Image Info ---" << std::endl;
    //image3D.PrintPicture();

    //// --- Извлечение и сохранение Z-слайса ---
    //Picture z_slice;
    //// Возьмем 10-й слайс по глубине (ось Z)
    //if (image3D.extractSlice(z_slice, SliceAxis::Z, 10)) {
    //    std::cout << "\n--- Z-Slice Info ---" << std::endl;
    //    z_slice.PrintPicture();
    //    z_slice.saveToFile(path + "z_slice_10.raw");
    //}

    //// --- Извлечение и сохранение Y-слайса ---
    //Picture y_slice;
    //// Возьмем 25-й горизонтальный слайс (ось Y)
    //if (image3D.extractSlice(y_slice, SliceAxis::Y, 25)) {
    //    std::cout << "\n--- Y-Slice Info ---" << std::endl;
    //    y_slice.PrintPicture();
    //    y_slice.saveToFile(path + "y_slice_25.raw");
    //}

    //// --- Извлечение и сохранение X-слайса ---
    //Picture x_slice;
    //// Возьмем 30-й вертикальный слайс (ось X)
    //if (image3D.extractSlice(x_slice, SliceAxis::X, 30)) {
    //    std::cout << "\n--- X-Slice Info ---" << std::endl;
    //    x_slice.PrintPicture();
    //    x_slice.saveToFile(path + "x_slice_30.raw");
    //}


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
