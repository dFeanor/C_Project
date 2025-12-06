#pragma once

#include <string>
#include <vector>

#include "Picture.h"
#include "Matrix.h" // Dense Matrix from Module 2
#include "CSR3.h"   // Sparse Matrix implementation

namespace FluidDynamics {

    enum class CellType { FLUID, SOLID };

    class StokesSolver {
    public:
        // Constructor: initializes geometry and pressure drop. Viscosity is fixed at 1.0.
        StokesSolver(const Picture& picture, double dP);

        // Main Uzawa algorithm loop
        void solve(double tolerance = 1e-6, int maxIterations = 10000, double alpha = 0.1);

        // Calculates absolute permeability using Darcy's law
        double calculatePermeability() const;

        // Saves pressure and velocity fields to a file
        void saveResults(const std::string& filename) const;

        // Validates solution against analytical Poiseuille flow (empty channel)
        double validatePoiseuille() const;

    private:
        // --- Geometry & Grid ---
        const Picture& geometry;
        uint64_t Nx, Ny;
        double h; // Grid resolution
        std::vector<std::vector<CellType>> domainMask;

        // --- Physics ---
        double deltaP;
        // mu = 1.0 (implicit)

        // --- Fields (MAC Staggered Grid) ---
        // Pressure at cell centers: size [Ny][Nx]
        Matrixes::Matrix pressure;

        // X-Velocity at vertical faces: size [Ny][Nx + 1]
        Matrixes::Matrix velocityX;

        // Y-Velocity at horizontal faces: size [Ny + 1][Nx]
        Matrixes::Matrix velocityY;

        // --- Linear System ---
        // Sparse matrix for the discrete Laplacian operator
        Matrixes::CSR3 systemMatrix;

        // --- Internal Helpers ---

        // parses Picture to domainMask
        void setupDomain();

        // Assembles the sparse Laplacian matrix (done once)
        void buildSystemMatrix();

        // Solves A * u = RHS - grad(p) for velocity components
        void solveMomentumEquation();

        // Computes divergence of velocity field
        Matrixes::Matrix computeDivergence() const;

        // Updates pressure: P_new = P_old - alpha * div(u)
        void updatePressure(const Matrixes::Matrix& divergence, double alpha);
    };

} // namespace FluidDynamics
