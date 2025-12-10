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

int main() {
    string dir = "picture_results";
    if (!fs::exists(dir)) fs::create_directory(dir);

    // Создание файла конфигурации перед запуском
    {
        ofstream config("config_dp.raw", ios::binary);
        if (config.is_open()) {
            double dp_value = 10.0;
            config.write(reinterpret_cast<const char*>(&dp_value), sizeof(double));
            config.close();
            cout << "Config file 'config_dp.raw' created with dP = " << dp_value << endl;
        }
        else {
            cerr << "Failed to create config file" << endl;
            return 1;
        }
    }

    try {
        double myDP = FluidDynamics::StokesSolver::readDeltaP("config_dp.raw");

        {
            cout << "TASK 1: Poiseuille" << endl;
            Picture p; p.createCylinder(40, 15, 0.0); p.addWalls();
            FluidDynamics::StokesSolver s(p, myDP);
            s.solve(1e-6, 10000);
            cout << "Error: " << s.validatePoiseuille() * 100 << "%" << endl;
            s.saveResults(dir + "/case1_num");
            // Используем myDP для аналитического решения, чтобы сравнение было корректным
            saveAnalytical(dir + "/case1_ana_mag.raw", p.getDim1(), p.getDim2(), 1.0, myDP);
        }

        {
            cout << "TASK 2: Pore" << endl;
            Picture p; p.createCylinderWithPore(40, 40, 18.0, 6.0);
            p.removeIsolatedPores(); p.addWalls();
            FluidDynamics::StokesSolver s(p, myDP + 5.0);
            s.solve(1e-2, 3000);
            s.saveResults(dir + "/case2_pore");
        }
        /*
        {
            cout << "TASK 3: Sinusoid" << endl;
            Picture p; p.createTortuousChannel(50, 25, 8.0, 5.0, 0.15);
            p.addWalls();
            FluidDynamics::StokesSolver s(p, myDP + 10.0);
            s.solve(1e-2, 5000);
            s.saveResults(dir + "/case3_sin");
        }
        */
        cout << "Done. Results in " << dir << endl;

    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}