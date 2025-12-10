#pragma once
#ifndef STOKES_SOLVER_H
#define STOKES_SOLVER_H

#include <vector>
#include <string>
#include "Picture.h"
#include "Matrix.h" 
#include "CSR3.h"   

using namespace std;

namespace FluidDynamics {

    enum class CellType { FLUID, SOLID };

    class StokesSolver {
    public:
        StokesSolver(const Picture& picture, double dP);

        static double readDeltaP(const std::string& filename);

        void solve(double tolerance = 1e-6, int maxIterations = 20000);

        double calculatePermeability() const;
        double validatePoiseuille() const;
        
        void saveResults(const string& prefix) const;
        
        bool checkMatrixSymmetry(double tol = 1e-10) const;

    private:
        const Picture& geometry;
        uint64_t Nx, Ny;
        double h;
        double deltaP;

        vector<vector<CellType>> domainMask;

        Matrixes::Matrix pressure;
        Matrixes::Matrix velocityX;
        Matrixes::Matrix velocityY;

        Matrixes::CSR3 systemMatrix;

        void setupDomain();
        void buildSystemMatrix();
        void solveMomentumEquation();
        Matrixes::Matrix computeDivergence() const;
        void updatePressure(const Matrixes::Matrix& divergence);
        void applyWallBoundaryConditions();
        bool isUxActive(uint64_t i, uint64_t j) const;
        bool isUyActive(uint64_t i, uint64_t j) const;
    };
}
#endif