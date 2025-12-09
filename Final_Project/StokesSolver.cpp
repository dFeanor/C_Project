#include "StokesSolver.h"

#include <string>
#include <vector>

using namespace FluidDynamics;
using namespace Matrixes;

void StokesSolver::setupDomain() {
    // Resize the mask to match grid dimensions
    domainMask.resize(Ny, std::vector<CellType>(Nx));

    const unsigned char* rawPixels = geometry.getPixelData();

    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            // Get pixel value from 1D array
            unsigned char pixelValue = rawPixels[i * Nx + j];

            // Determine cell type based on pixel value (0 = Pore/Fluid, 255 = Rock/Solid)
            if (pixelValue == 0) {
                domainMask[i][j] = CellType::FLUID;
            }
            else {
                domainMask[i][j] = CellType::SOLID;
            }
        }
    }
}

void StokesSolver::buildSystemMatrix() {
    uint64_t numU = Ny * (Nx + 1);
    uint64_t numV = (Ny + 1) * Nx;
    uint64_t totalUnknowns = numU + numV;

    systemMatrix.M = totalUnknowns;
    systemMatrix.N = totalUnknowns;
    systemMatrix.rowIndex.assign(totalUnknowns + 1, 0);
    systemMatrix.values.clear();
    systemMatrix.columns.clear();

    double invH2 = 1.0 / (h * h);

    // =========================================================
    // 1. U-COMPONENTS (Horizontal Velocity)
    // defined at vertical faces between cell(i, j-1) and cell(i, j)
    // =========================================================
    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx + 1; ++j) {
            int currentRow = i * (Nx + 1) + j;

            // --- 1.1 INLET (Neumann) ---
            if (j == 0) {
                systemMatrix.values.push_back(1.0);
                systemMatrix.columns.push_back(currentRow);
                systemMatrix.values.push_back(-1.0);
                systemMatrix.columns.push_back(currentRow + 1);
            }
            // --- 1.2 OUTLET (Neumann) ---
            else if (j == Nx) {
                systemMatrix.values.push_back(-1.0);
                systemMatrix.columns.push_back(currentRow - 1);
                systemMatrix.values.push_back(1.0);
                systemMatrix.columns.push_back(currentRow);
            }
            // --- 1.3 INTERNAL DOMAIN ---
            else {
                // A face is strictly FLUID only if BOTH adjacent cells are fluid.
                // If any is solid, it's a wall or inside rock -> U=0.
                bool isFluidFace = (domainMask[i][j - 1] == CellType::FLUID && domainMask[i][j] == CellType::FLUID);

                if (!isFluidFace) {
                    // Solid/Wall: U = 0
                    systemMatrix.values.push_back(1.0);
                    systemMatrix.columns.push_back(currentRow);
                }
                else {
                    // Fluid: Discrete Laplacian
                    double diagVal = 4.0;

                    // --- CHECK NEIGHBORS ---

                    // 1. Top Neighbor (Shear direction - Ghost Point Logic)
                    // Neighbor is U[i-1][j]. It is strictly valid only if cells (i-1, j-1) and (i-1, j) are fluid.
                    bool topIsSolid = false;
                    if (i == 0) topIsSolid = true; // Domain boundary
                    else if (domainMask[i - 1][j - 1] == CellType::SOLID || domainMask[i - 1][j] == CellType::SOLID) topIsSolid = true;

                    if (topIsSolid) diagVal += 1.0; // Ghost point: u_ghost = -u_curr

                    // 2. Bottom Neighbor (Shear direction - Ghost Point Logic)
                    bool botIsSolid = false;
                    if (i == Ny - 1) botIsSolid = true;
                    else if (domainMask[i + 1][j - 1] == CellType::SOLID || domainMask[i + 1][j] == CellType::SOLID) botIsSolid = true;

                    if (botIsSolid) diagVal += 1.0; // Ghost point

                    // 3. Left Neighbor (Normal direction - Dirichlet Logic)
                    // Neighbor is U[i][j-1]. It is face between (j-2) and (j-1).
                    // We are at face (j-1)|(j). If face (j-1) is not fluid-fluid, it is a wall node U=0.
                    // We just don't add the neighbor (implicitly +0), Diag DOES NOT change.
                    bool leftIsSolid = false;
                    if (j - 1 == 0) {/*Inlet is ok*/ }
                    else if (domainMask[i][j - 2] == CellType::SOLID) leftIsSolid = true;

                    // 4. Right Neighbor (Normal direction - Dirichlet Logic)
                    bool rightIsSolid = false;
                    if (j + 1 == Nx) {/*Outlet is ok*/ }
                    else if (domainMask[i][j + 1] == CellType::SOLID) rightIsSolid = true;


                    // --- FILL MATRIX ---

                    // Top
                    if (i > 0 && !topIsSolid) {
                        systemMatrix.values.push_back(-1.0 * invH2);
                        systemMatrix.columns.push_back((i - 1) * (Nx + 1) + j);
                    }
                    // Left
                    if (!leftIsSolid) {
                        systemMatrix.values.push_back(-1.0 * invH2);
                        systemMatrix.columns.push_back(i * (Nx + 1) + (j - 1));
                    }
                    // Diagonal
                    systemMatrix.values.push_back(diagVal * invH2);
                    systemMatrix.columns.push_back(currentRow);
                    // Right
                    if (!rightIsSolid) {
                        systemMatrix.values.push_back(-1.0 * invH2);
                        systemMatrix.columns.push_back(i * (Nx + 1) + (j + 1));
                    }
                    // Bottom
                    if (i < Ny - 1 && !botIsSolid) {
                        systemMatrix.values.push_back(-1.0 * invH2);
                        systemMatrix.columns.push_back((i + 1) * (Nx + 1) + j);
                    }
                }
            }
            systemMatrix.rowIndex[currentRow + 1] = systemMatrix.values.size();
        }
    }

    // =========================================================
    // 2. V-COMPONENTS (Vertical Velocity)
    // defined at horizontal faces between cell(i-1, j) and cell(i, j)
    // =========================================================
    for (uint64_t i = 0; i < Ny + 1; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {
            int currentRow = numU + i * Nx + j;

            bool isBoundary = (i == 0 || i == Ny || j == 0 || j == Nx - 1);
            bool isFluidFace = false;

            if (!isBoundary) {
                // Active V-face must have Fluid above and Fluid below
                isFluidFace = (domainMask[i - 1][j] == CellType::FLUID && domainMask[i][j] == CellType::FLUID);
            }

            if (isBoundary || !isFluidFace) {
                // V = 0
                systemMatrix.values.push_back(1.0);
                systemMatrix.columns.push_back(currentRow);
            }
            else {
                // Fluid: Discrete Laplacian for V
                double diagVal = 4.0;

                // 1. Left Neighbor (Shear - Ghost Point)
                // Check if V[i][j-1] is valid
                bool leftIsSolid = false;
                if (domainMask[i - 1][j - 1] == CellType::SOLID || domainMask[i][j - 1] == CellType::SOLID) leftIsSolid = true;

                if (leftIsSolid) diagVal += 1.0;

                // 2. Right Neighbor (Shear - Ghost Point)
                bool rightIsSolid = false;
                if (domainMask[i - 1][j + 1] == CellType::SOLID || domainMask[i][j + 1] == CellType::SOLID) rightIsSolid = true;

                if (rightIsSolid) diagVal += 1.0;

                // 3. Top Neighbor (Normal - Dirichlet)
                // Neighbor V[i-1][j]. If invalid, it's 0.
                bool topIsSolid = false;
                if (i - 1 == 0) {/*Boundary ok*/ }
                else if (domainMask[i - 2][j] == CellType::SOLID) topIsSolid = true;

                // 4. Bottom Neighbor (Normal - Dirichlet)
                bool botIsSolid = false;
                if (i + 1 == Ny) {/*Boundary ok*/ }
                else if (domainMask[i + 1][j] == CellType::SOLID) botIsSolid = true;

                // --- FILL MATRIX ---

                // Top
                if (!topIsSolid) {
                    systemMatrix.values.push_back(-1.0 * invH2);
                    systemMatrix.columns.push_back(numU + (i - 1) * Nx + j);
                }
                // Left
                if (j > 0 && !leftIsSolid) {
                    systemMatrix.values.push_back(-1.0 * invH2);
                    systemMatrix.columns.push_back(numU + i * Nx + (j - 1));
                }
                // Diagonal
                systemMatrix.values.push_back(diagVal * invH2);
                systemMatrix.columns.push_back(currentRow);
                // Right
                if (j < Nx - 1 && !rightIsSolid) {
                    systemMatrix.values.push_back(-1.0 * invH2);
                    systemMatrix.columns.push_back(numU + i * Nx + (j + 1));
                }
                // Bottom
                if (!botIsSolid) {
                    systemMatrix.values.push_back(-1.0 * invH2);
                    systemMatrix.columns.push_back(numU + (i + 1) * Nx + j);
                }
            }
            systemMatrix.rowIndex[currentRow + 1] = systemMatrix.values.size();
        }
    }

    systemMatrix.L = systemMatrix.values.size();
}

void StokesSolver::updatePressure(const Matrixes::Matrix& divergence) {
    // Iterate over all pressure cells (centers of the grid)
    for (uint64_t i = 0; i < Ny; ++i) {
        for (uint64_t j = 0; j < Nx; ++j) {

            // Optimization: Update pressure only in FLUID cells.
            // In SOLID cells, velocity is 0, so divergence is 0, and pressure doesn't change.
            // However, strictly following the formula implies operations on the whole domain.

            if (domainMask[i][j] == CellType::FLUID) {
                // Get current pressure p^k
                double currentP = pressure.get_elem(i, j);

                // Get divergence of the newly calculated velocity field
                double div = divergence.get_elem(i, j);

                // Update formula: p^{k+1} = p^k - div
                pressure.set_elem(i, j, currentP - div);
            }
        }
    }
}