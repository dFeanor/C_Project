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
    // Исправлено:
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

    try {
        {
            cout << "TASK 1: Poiseuille" << endl;
            Picture p; p.createCylinder(40, 15, 0.0); p.addWalls();
            FluidDynamics::StokesSolver s(p, 10.0);
            s.solve(1e-6, 10000);
            cout << "Error: " << s.validatePoiseuille() * 100 << "%" << endl;
            s.saveResults(dir + "/case1_num");
            saveAnalytical(dir + "/case1_ana_mag.raw", p.getDim1(), p.getDim2(), 1.0, 10.0);
        }

        {
            cout << "TASK 2: Pore" << endl;
            Picture p; p.createCylinderWithPore(40, 40, 18.0, 6.0);
            p.removeIsolatedPores(); p.addWalls();
            FluidDynamics::StokesSolver s(p, 15.0);
            s.solve(1e-4, 3000);
            s.saveResults(dir + "/case2_pore");
        }

        {
            cout << "TASK 3: Sinusoid" << endl;
            Picture p; p.createTortuousChannel(50, 25, 8.0, 5.0, 0.15);
            p.addWalls();
            FluidDynamics::StokesSolver s(p, 20.0);
            s.solve(1e-4, 5000);
            s.saveResults(dir + "/case3_sin");
        }

        {
            cout << "TASK 4: 2D Raw" << endl;
            if (fs::exists("picture_2d.raw")) {
                Picture p; 
                if (p.loadFromFile("picture_2d.raw", false)) {
                    p.removeIsolatedPores(); p.addWalls();
                    FluidDynamics::StokesSolver s(p, 10.0);
                    s.solve(1e-4, 5000);
                    s.saveResults(dir + "/case4_2d");
                }
            } else {
                cout << "picture_2d.raw not found" << endl;
            }
        }

        {
            cout << "TASK 5: 3D Slice" << endl;
            if (fs::exists("picture_3d.raw")) {
                Picture p3; 
                if (p3.loadFromFile("picture_3d.raw", true)) {
                    Picture p; 
                    if (p3.extractSlice(p, SliceAxis::Z, p3.getDim3()/2)) {
                        p.removeIsolatedPores(); p.addWalls();
                        FluidDynamics::StokesSolver s(p, 10.0);
                        s.solve(1e-4, 5000);
                        s.saveResults(dir + "/case5_3d");
                    }
                }
            } else {
                cout << "picture_3d.raw not found" << endl;
            }
        }

        cout << "Done. Results in " << dir << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    return 0;
}