#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <filesystem>
#include <stdexcept>

#include "Picture.h"
#include "StokesSolver.h"

using namespace std;
namespace fs = std::filesystem;

void saveAnalytical(const string& filename, int Ny, int Nx, double h, double dP) {
    ofstream f(filename, ios::binary);
    f.write((char*)&Ny, 4); f.write((char*)&Nx, 4);

    int y1 = 1;
    int y2 = Ny - 2;
    double H = (y2 - y1 + 1) * h;
    double G = abs(dP) / ((Nx - 1) * h);

    for (int i = 0; i < Ny; ++i) {
        for (int j = 0; j < Nx; ++j) {
            double val = 0.0;
            if (i >= y1 && i <= y2) {
                double y = (i - y1 + 0.5) * h;
                val = (G / 2.0) * y * (H - y);
            }
            f.write((char*)&val, 8);
        }
    }
}

void savePermeabilityToFile(const string& filename, double k) {
    ofstream f(filename);
    if (f.is_open()) {
        f << "Absolute Permeability: " << scientific << k << endl;
        f.close();
        cout << "Saved permeability to " << filename << " (k = " << k << ")" << endl;
    }
    else {
        cerr << "Failed to save permeability to " << filename << endl;
    }
}
// ---------------------


int main() {
    string dir = "picture_results";
    if (!fs::exists(dir)) fs::create_directory(dir);

    // Создание файла конфигурации (как в прошлом шаге)
    {
        ofstream config("config_dp.bin", ios::binary);
        if (config.is_open()) {
            double dp_value = 10.0;
            config.write(reinterpret_cast<const char*>(&dp_value), sizeof(double));
            config.close();
        }
    }

    try {
        double myDP = FluidDynamics::StokesSolver::readDeltaP("config_dp.bin");

        // --- TASK 1 ---
        {
            cout << "\nTASK 1: Poiseuille" << endl;
            Picture p; p.createCylinder(40, 15, 0.0); p.addWalls();
            FluidDynamics::StokesSolver s(p, myDP);
            s.solve(1e-6, 10000); // Высокая точность для проверки ошибки

            cout << "Error: " << s.validatePoiseuille() * 100 << "%" << endl;
            s.saveResults(dir + "/case1_num");
            saveAnalytical(dir + "/case1_ana_mag.raw", p.getDim1(), p.getDim2(), 1.0, myDP);

            // Расчет и сохранение проницаемости
            double k = s.calculatePermeability();
            savePermeabilityToFile(dir + "/case1_perm.txt", k);
        }

        // --- TASK 2 ---
        {
            cout << "\nTASK 2: Pore" << endl;
            Picture p; p.createCylinderWithPore(40, 40, 18.0, 6.0);
            p.removeIsolatedPores(); p.addWalls();
            FluidDynamics::StokesSolver s(p, myDP + 5.0);
            s.solve(1e-2, 3000); // Ослабленная точность для скорости
            s.saveResults(dir + "/case2_pore");

            // Расчет и сохранение проницаемости
            double k = s.calculatePermeability();
            savePermeabilityToFile(dir + "/case2_perm.txt", k);
        }

        // --- TASK 3 ---
        {
            cout << "\nTASK 3: Sinusoid" << endl;
            Picture p; p.createTortuousChannel(50, 25, 8.0, 5.0, 0.15);
            p.addWalls();
            FluidDynamics::StokesSolver s(p, myDP + 10.0);
            s.solve(1e-2, 5000);
            s.saveResults(dir + "/case3_sin");

            // Расчет и сохранение проницаемости
            double k = s.calculatePermeability();
            savePermeabilityToFile(dir + "/case3_perm.txt", k);
        }

        cout << "\nDone. Results in " << dir << endl;

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}