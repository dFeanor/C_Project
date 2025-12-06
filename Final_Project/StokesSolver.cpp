#include "StokesSolver.h"

#include <string>
#include <vector>

using namespace FluidDynamics;

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