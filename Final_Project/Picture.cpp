#include "Picture.h"
#include <fstream>
#include <iostream>

Picture::Picture() : N1(0), N2(0), N3(0), resolution(0.0), pixels(nullptr) {}

Picture::~Picture() {
    delete[] pixels; 
}

bool Picture::loadFromFile(const std::string& filePath, bool is3D) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&N1), sizeof(N1));
    file.read(reinterpret_cast<char*>(&N2), sizeof(N2));

    if (is3D) {
        file.read(reinterpret_cast<char*>(&N3), sizeof(N3));
    }
    else {
        N3 = 0; 
    }

    file.read(reinterpret_cast<char*>(&resolution), sizeof(resolution));

    if (!file) {
        std::cerr << "Error: Failed to read header from file " << filePath << std::endl;
        return false;
    }

    wallsAdded = false;

    size_t totalPixels = is3D ? (N1 * N2 * N3) : (N1 * N2);
    if (totalPixels == 0) {
        std::cerr << "Error: Image dimensions are zero." << std::endl;
        return false;
    }

    delete[] pixels;

    pixels = new unsigned char[totalPixels];

    file.read(reinterpret_cast<char*>(pixels), totalPixels);

    if (!file) {
        std::cerr << "Error: Failed to read pixel data from file " << filePath << std::endl;
        delete[] pixels; 
        pixels = nullptr;
        return false;
    }

    std::cout << "Successfully loaded image " << filePath << std::endl;
    return true;
}

bool Picture::saveToFile(const std::string& filePath) const {
    if (pixels == nullptr) {
        std::cerr << "Error: Cannot save an empty picture." << std::endl;
        return false;
    }
    if (N3 > 0) {
        std::cerr << "Error: saveToFile is implemented for 2D images only." << std::endl;
        return false;
    }

    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filePath << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(&N1), sizeof(N1));
    file.write(reinterpret_cast<const char*>(&N2), sizeof(N2));
    file.write(reinterpret_cast<const char*>(&resolution), sizeof(resolution));

    uint64_t totalPixels = N1 * N2;
    file.write(reinterpret_cast<const char*>(pixels), totalPixels);

    if (!file) {
        std::cerr << "Error: An error occurred while writing to file: " << filePath << std::endl;
        return false;
    }

    std::cout << "Successfully saved image to " << filePath << std::endl;
    return true;
}


bool Picture::extractSubregion(Picture& outputPicture, uint64_t startX, uint64_t startY, uint64_t size) const {

    if (pixels == nullptr) {
        std::cerr << "Error: Cannot extract subregion from an empty picture. Load a file first." << std::endl;
        return false;
    }
    if (N3 > 0) {
        std::cerr << "Error: extractSubregion is implemented for 2D images only. Current image is 3D." << std::endl;
        return false;
    }

    if (size == 0) {
        std::cerr << "Error: Subregion size cannot be zero." << std::endl;
        return false;
    }
    if (startX >= N2 || startY >= N1) {
        std::cerr << "Error: Start coordinates (" << startX << ", " << startY << ") are out of bounds." << std::endl;
        return false;
    }
    if (startX + size > N2 || startY + size > N1) {
        std::cerr << "Error: Subregion (" << startX << "," << startY << ") with size " << size
            << " extends beyond image dimensions (" << N2 << ", " << N1 << ")." << std::endl;
        return false;
    }

    if (outputPicture.pixels != nullptr) {
        std::cerr << "Error: Output picture is not empty. Please provide an empty Picture object." << std::endl;
        return false;
    }
    uint64_t new_N1 = size;
    uint64_t new_N2 = size;
    uint64_t newTotalPixels = new_N1 * new_N2;

    unsigned char* newPixelsBuffer = new (std::nothrow) unsigned char[newTotalPixels];
    if (newPixelsBuffer == nullptr) {
        std::cerr << "Error: Failed to allocate memory for subregion pixels." << std::endl;
        return false;
    }

    for (uint64_t r = 0; r < new_N1; ++r) {
        uint64_t sourceRowStartIdx = (startY + r) * N2 + startX;
        uint64_t destRowStartIdx = r * new_N2;

        std::memcpy(newPixelsBuffer + destRowStartIdx,
            pixels + sourceRowStartIdx,
            new_N2 * sizeof(unsigned char));
    }

    outputPicture.N1 = new_N1;
    outputPicture.N2 = new_N2;
    outputPicture.N3 = 0;
    outputPicture.resolution = this->resolution;
    outputPicture.pixels = newPixelsBuffer;
    outputPicture.wallsAdded = false;

    std::cout << "Successfully extracted subregion (" << startX << ", " << startY << ") with size " << size
        << ". New dimensions: " << new_N1 << "x" << new_N2 << std::endl;
    return true;
}

bool Picture::extractSlice(Picture& outputSlice, SliceAxis axis, uint64_t sliceIndex) const {
    if (pixels == nullptr) {
        std::cerr << "Error: Cannot extract a slice from an empty picture." << std::endl;
        return false;
    }
    if (N3 == 0) {
        std::cerr << "Error: Slices can only be extracted from 3D images." << std::endl;
        return false;
    }
    if (outputSlice.pixels != nullptr) {
        std::cerr << "Error: Output picture for the slice must be empty." << std::endl;
        return false;
    }

    uint64_t new_N1 = 0, new_N2 = 0;

    switch (axis) {
    case SliceAxis::X:
        if (sliceIndex >= N2) {
            std::cerr << "Error: X-axis slice index " << sliceIndex << " is out of bounds [0, " << N2 - 1 << "]." << std::endl;
            return false;
        }
        new_N1 = N1; 
        new_N2 = N3; 
        break;
    case SliceAxis::Y:
        if (sliceIndex >= N1) {
            std::cerr << "Error: Y-axis slice index " << sliceIndex << " is out of bounds [0, " << N1 - 1 << "]." << std::endl;
            return false;
        }
        new_N1 = N2; 
        new_N2 = N3; 
        break;
    case SliceAxis::Z:
        if (sliceIndex >= N3) {
            std::cerr << "Error: Z-axis slice index " << sliceIndex << " is out of bounds [0, " << N3 - 1 << "]." << std::endl;
            return false;
        }
        new_N1 = N1;
        new_N2 = N2;
        break;
    }

    uint64_t totalNewPixels = new_N1 * new_N2;
    unsigned char* newPixels = new (std::nothrow) unsigned char[totalNewPixels];
    if (newPixels == nullptr) {
        std::cerr << "Error: Failed to allocate memory for the slice." << std::endl;
        return false;
    }

    uint64_t currentNewPixel = 0;
    switch (axis) {
    case SliceAxis::Z: { 
        uint64_t offset = sliceIndex * (N1 * N2);
        std::memcpy(newPixels, pixels + offset, totalNewPixels);
        break;
    }
    case SliceAxis::Y: { 
        for (uint64_t k = 0; k < N3; ++k) {
            for (uint64_t j = 0; j < N2; ++j) {
                uint64_t sourceIndex = k * (N1 * N2) + sliceIndex * N2 + j;
                newPixels[currentNewPixel++] = pixels[sourceIndex];
            }
        }
        break;
    }
    case SliceAxis::X: {
        for (uint64_t k = 0; k < N3; ++k) {
            for (uint64_t i = 0; i < N1; ++i) {
                uint64_t sourceIndex = k * (N1 * N2) + i * N2 + sliceIndex;
                newPixels[currentNewPixel++] = pixels[sourceIndex];
            }
        }
        break;
    }
    }

    outputSlice.N1 = new_N1;
    outputSlice.N2 = new_N2;
    outputSlice.N3 = 0; 
    outputSlice.resolution = this->resolution;
    outputSlice.pixels = newPixels;
    outputSlice.wallsAdded = false;

    std::cout << "Successfully extracted slice." << std::endl;
    return true;
}

void Picture::addWalls() {
    if (pixels == nullptr) {
        std::cerr << "Warning: Cannot add walls to an empty picture. Load a file first." << std::endl;
        return;
    }
    if (wallsAdded) {
        std::cout << "Info: Walls have already been added to this picture." << std::endl;
        return;
    }
    if (N3 > 0) {
        std::cerr << "Warning: The 'addTopBottomWalls' method is implemented for 2D images only." << std::endl;
        return;
    }

    std::cout << "Adding top and bottom walls..." << std::endl;

    const unsigned char WALL_PIXEL_VALUE = 255;

    uint64_t old_N1 = N1;    
    uint64_t new_N1 = N1 + 2;

    uint64_t rowSizeBytes = N2; 
    uint64_t oldTotalPixels = old_N1 * N2;
    uint64_t newTotalPixels = new_N1 * N2;

    unsigned char* newPixels = new unsigned char[newTotalPixels];

    std::memset(newPixels, WALL_PIXEL_VALUE, rowSizeBytes);

    std::memcpy(newPixels + rowSizeBytes, pixels, oldTotalPixels);

    std::memset(newPixels + rowSizeBytes + oldTotalPixels, WALL_PIXEL_VALUE, rowSizeBytes);

    delete[] pixels;

    pixels = newPixels; 
    N1 = new_N1;        
    wallsAdded = true;  

    std::cout << "Successfully added walls. New dimensions (N1 x N2): " << N1 << " x " << N2 << std::endl;
}

size_t Picture::getDim1() const {
    return N1;
}

size_t Picture::getDim2() const {
    return N2;
}

size_t Picture::getDim3() const {
    return N3;
}

double Picture::getResolution() const {
    return resolution;
}

size_t Picture::getTotalPixels() const {
    return N3 > 0 ? (N1 * N2 * N3) : (N1 * N2);
}

const unsigned char* Picture::getPixelData() const {
    return pixels;
}

void Picture::PrintPicture() {
    std::cout << "--- Picture Info ---" << std::endl;

    if (pixels == nullptr) {
        std::cout << "Picture is empty or not loaded." << std::endl;
        std::cout << "--------------------" << std::endl;
        return; 
    }

    bool is3D = (N3 > 0);

    if (is3D) {
        std::cout << "Type: 3D" << std::endl;
        std::cout << "Dimensions (N1 x N2 x N3): " << getDim1() << " x " << getDim2() << " x " << getDim3() << std::endl;
    }
    else {
        std::cout << "Type: 2D" << std::endl;
        std::cout << "Dimensions (N1 x N2): " << getDim1() << " x " << getDim2() << std::endl;
    }

    std::cout << "Resolution: " << getResolution() << std::endl;

    uint64_t totalPixels = getTotalPixels();
    std::cout << "Total Pixels: " << totalPixels << std::endl;

    std::cout << "Sample of first pixels: ";
    if (totalPixels > 0) {
        uint64_t pixelsToShow = std::min(totalPixels, static_cast<uint64_t>(16));
        for (uint64_t i = 0; i < pixelsToShow; ++i) {
            std::cout << static_cast<int>(pixels[i]) << " ";
        }
    }
    else {
        std::cout << "No pixel data.";
    }

    std::cout << std::endl;
    std::cout << "--------------------" << std::endl;
}
