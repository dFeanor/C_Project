#include "StokesSolver.h"
#include "SLAY.h"

#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <map>
#include <stdexcept>

using namespace FluidDynamics;
using namespace Matrixes;
using namespace std;

StokesSolver::StokesSolver(const Picture& picture, double dP)
    : geometry(picture), deltaP(dP),
      pressure(picture.getDim1(), picture.getDim2(), 0.0),
      velocityX(picture.getDim1(), picture.getDim2() + 1, 0.0),
      velocityY(picture.getDim1() + 1, picture.getDim2(), 0.0)
{
    Ny = picture.getDim1();
    Nx = picture.getDim2();
    h = picture.getResolution();

    if (Ny == 0 || Nx == 0) throw invalid_argument("Dimensions are zero");
    if (h <= 1e-9) throw invalid_argument("Resolution too small");

    // Инициализация давления линейно (для ускорения сходимости)
    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            pressure.set_elem(i, j, deltaP * (1.0 - (double)j / (Nx - 1)));
        }
    }

    setupDomain();
    
    try {
        buildSystemMatrix();
    } catch (const exception& e) {
        throw runtime_error(string("Matrix Build Error: ") + e.what());
    }

    if (!checkMatrixSymmetry()) {
        throw runtime_error("Matrix is not symmetric");
    }
}

void StokesSolver::setupDomain() {
    domainMask.resize(Ny, vector<CellType>(Nx));
    const unsigned char* rawPixels = geometry.getPixelData();
    if (!rawPixels) throw runtime_error("Pixel data null");

    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            if (rawPixels[i * Nx + j] < 128) domainMask[i][j] = CellType::FLUID;
            else domainMask[i][j] = CellType::SOLID;
        }
    }
}

bool StokesSolver::isUxActive(uint64_t i, uint64_t j) const {
    if (j == 0) return (domainMask[i][0] == CellType::FLUID);
    if (j == Nx) return (domainMask[i][Nx - 1] == CellType::FLUID);
    return (domainMask[i][j - 1] == CellType::FLUID && domainMask[i][j] == CellType::FLUID);
}

bool StokesSolver::isUyActive(uint64_t i, uint64_t j) const {
    if (i == 0 || i == Ny) return false;
    return (domainMask[i - 1][j] == CellType::FLUID && domainMask[i][j] == CellType::FLUID);
}

void StokesSolver::buildSystemMatrix() {
    uint64_t numU = Ny * (Nx + 1);
    uint64_t numV = (Ny + 1) * Nx;
    uint64_t total = numU + numV;

    systemMatrix.M = total;
    systemMatrix.N = total;
    systemMatrix.rowIndex.assign(total + 1, 0);
    systemMatrix.values.clear();
    systemMatrix.columns.clear();

    double scale = 1.0 / (h * h);
    vector<map<int, double>> rows(total);

    auto add = [&](int r, int c, double v) { rows[r][c] += v; };

    // Ux
    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx + 1; ++j) {
            int row = i * (Nx + 1) + j;
            if (!isUxActive(i, j)) {
                add(row, row, 1.0);
            } else {
                if (j == 0) { // Inlet
                    add(row, row, scale);
                    if (isUxActive(i, j + 1)) add(row, row + 1, -scale);
                } else if (j == Nx) { // Outlet
                    add(row, row, scale);
                    if (isUxActive(i, j - 1)) add(row, row - 1, -scale);
                } else {
                    add(row, row, 4.0 * scale);
                    if (isUxActive(i, j - 1)) add(row, i*(Nx+1)+(j-1), -scale);
                    if (isUxActive(i, j + 1)) add(row, i*(Nx+1)+(j+1), -scale);
                    if (i > 0 && isUxActive(i - 1, j)) add(row, (i-1)*(Nx+1)+j, -scale);
                    if (i < Ny - 1 && isUxActive(i + 1, j)) add(row, (i+1)*(Nx+1)+j, -scale);
                }
            }
        }
    }

    // Uy
    for (uint64_t i = 0; i < Ny + 1; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            int row = numU + i * Nx + j;
            if (!isUyActive(i, j)) {
                add(row, row, 1.0);
            } else {
                add(row, row, 4.0 * scale);
                if (i > 0 && isUyActive(i - 1, j)) add(row, numU+(i-1)*Nx+j, -scale);
                if (i < Ny && isUyActive(i + 1, j)) add(row, numU+(i+1)*Nx+j, -scale);
                if (j > 0 && isUyActive(i, j - 1)) add(row, numU+i*Nx+(j-1), -scale);
                if (j < Nx - 1 && isUyActive(i, j + 1)) add(row, numU+i*Nx+(j+1), -scale);
            }
        }
    }

    systemMatrix.rowIndex[0] = 0;
    int count = 0;
    for (int i = 0; i < total; ++i) {
        for (auto const& [c, v] : rows[i]) {
            systemMatrix.values.push_back(v);
            systemMatrix.columns.push_back(c);
            count++;
        }
        systemMatrix.rowIndex[i + 1] = count;
    }
    systemMatrix.L = count;
}

bool StokesSolver::checkMatrixSymmetry(double tol) const {
    map<pair<int,int>, double> els;
    for(int i=0; i<systemMatrix.M; ++i)
        for(int k=systemMatrix.rowIndex[i]; k<systemMatrix.rowIndex[i+1]; ++k)
            els[{i, systemMatrix.columns[k]}] = systemMatrix.values[k];

    for(auto const& [kc, val] : els) {
        if (kc.first == kc.second) continue;
        if (els.find({kc.second, kc.first}) == els.end()) return false;
        if (abs(val - els[{kc.second, kc.first}]) > tol) return false;
    }
    return true;
}

void StokesSolver::solveMomentumEquation() {
    uint64_t numU = Ny * (Nx + 1);
    uint64_t numV = (Ny + 1) * Nx;
    uint64_t total = numU + numV;
    vector<double> rhs(total, 0.0);

    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 1; j < Nx; ++j) { 
            if (isUxActive(i, j)) {
                rhs[i*(Nx+1)+j] = (pressure.get_elem(i, j-1) - pressure.get_elem(i, j)) / h;
            }
        }
    }
    for (uint64_t i = 1; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            if (isUyActive(i, j)) {
                rhs[numU+i*Nx+j] = (pressure.get_elem(i-1, j) - pressure.get_elem(i, j)) / h;
            }
        }
    }

    vector<double> sol = SLAYSolver::solve_cholesky(systemMatrix, rhs);

    for (uint64_t i = 0; i < Ny; ++i)
        for (uint64_t j = 0; j < Nx + 1; ++j)
            velocityX.set_elem(i, j, sol[i * (Nx + 1) + j]);

    for (uint64_t i = 0; i < Ny + 1; ++i)
        for (uint64_t j = 0; j < Nx; ++j)
            velocityY.set_elem(i, j, sol[numU + i * Nx + j]);
}

Matrix StokesSolver::computeDivergence() const {
    Matrix div(Ny, Nx, 0.0);
    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            if (domainMask[i][j] == CellType::FLUID) {
                double ux_r = velocityX.get_elem(i, j + 1);
                double ux_l = velocityX.get_elem(i, j);
                double uy_b = velocityY.get_elem(i + 1, j);
                double uy_t = velocityY.get_elem(i, j);
                div.set_elem(i, j, (ux_r - ux_l)/h + (uy_b - uy_t)/h);
            }
        }
    }
    return div;
}

void StokesSolver::updatePressure(const Matrix& div) {
    double alpha = 1.0; 
    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 1; j < Nx - 1; ++j) {
            if (domainMask[i][j] == CellType::FLUID) {
                double p = pressure.get_elem(i, j);
                double d = div.get_elem(i, j);
                pressure.set_elem(i, j, p - alpha * d);
            }
        }
    }
}

void StokesSolver::solve(double tol, int maxIter) {
    cout << "Stokes Solver Started" << endl;
    for (int k = 0; k < maxIter; ++k) {
        solveMomentumEquation();
        Matrix div = computeDivergence();
        
        double maxD = 0.0;
        for (uint64_t i = 0; i < Ny; ++i)
            for (uint64_t j = 0; j < Nx; ++j)
                if (domainMask[i][j] == CellType::FLUID)
                    maxD = max(maxD, abs(div.get_elem(i, j)));

        if (maxD < tol) {
            cout << "Converged at " << k << ", Div: " << maxD << endl;
            break;
        }
        updatePressure(div);
    }
}

double StokesSolver::calculatePermeability() const {
    if (abs(deltaP) < 1e-9) return 0.0;
    double Q = 0.0;
    int jc = Nx / 2;
    for (uint64_t i = 0; i < Ny; ++i) Q += velocityX.get_elem(i, jc) * h;
    return (abs(Q) * 1.0 * (Nx * h)) / ((Ny * h) * abs(deltaP));
}

double StokesSolver::validatePoiseuille() const {
    double err2 = 0, ana2 = 0;
    int jc = Nx / 2;
    int y1 = 0; while(y1 < Ny && domainMask[y1][jc] == CellType::SOLID) y1++;
    int y2 = Ny-1; while(y2 >= 0 && domainMask[y2][jc] == CellType::SOLID) y2--;
    if (y1 > y2) return 0.0;
    
    double H_eff = (y2 - y1 + 1) * h;
    // Исправлено: расстояние для градиента = (Nx-1)*h
    double gradP = abs(deltaP) / ((Nx - 1) * h);

    for (uint64_t i = 0; i < Ny; ++i) {
        if (domainMask[i][jc] == CellType::FLUID) {
            double u_num = velocityX.get_elem(i, jc);
            double y = (i - y1 + 0.5) * h;
            double u_ana = (gradP / 2.0) * y * (H_eff - y);
            err2 += pow(u_num - u_ana, 2);
            ana2 += pow(u_ana, 2);
        }
    }
    return (ana2 > 0) ? sqrt(err2/ana2) : 0.0;
}

void StokesSolver::saveResults(const string& prefix) const {
    auto save = [&](string suffix, const Matrix& m, int R, int C) {
        ofstream f(prefix + suffix, ios::binary);
        f.write((char*)&R, 4); f.write((char*)&C, 4);
        for(int i=0; i<R; ++i)
            for(int j=0; j<C; ++j) {
                double v = m.get_elem(i, j);
                f.write((char*)&v, 8);
            }
    };

    save("_pressure.raw", pressure, Ny, Nx);
    save("_ux.raw", velocityX, Ny, Nx+1);
    save("_uy.raw", velocityY, Ny+1, Nx);

    ofstream fm(prefix + "_mag.raw", ios::binary);
    int r = Ny, c = Nx;
    fm.write((char*)&r, 4); fm.write((char*)&c, 4);
    for(int i=0; i<r; ++i) {
        for (int j=0; j<c; ++j) {
            double ux = 0.5 * (velocityX.get_elem(i, j) + velocityX.get_elem(i, j+1));
            double uy = 0.5 * (velocityY.get_elem(i, j) + velocityY.get_elem(i+1, j));
            double mag = sqrt(ux*ux + uy*uy);
            fm.write((char*)&mag, 8);
        }
    }
    cout << "Saved: " << prefix << "*.raw" << endl;
}