#ifndef PICTURE_H
#define PICTURE_H

#include <string>
#include <vector>
#include <cstdint>

enum class SliceAxis {
    X, Y, Z
};

class Picture {
public:
    Picture();
    ~Picture();

    bool loadFromFile(const std::string& filePath, bool is3D = false);

    bool saveToFile(const std::string& filePath) const;

    bool extractSubregion(Picture& outputPicture, uint64_t startX, uint64_t startY, uint64_t size) const;
    bool extractSlice(Picture& outputSlice, SliceAxis axis, uint64_t sliceIndex) const;

    uint64_t getDim1() const;
    uint64_t getDim2() const;
    uint64_t getDim3() const;

    double getResolution() const;
    uint64_t getTotalPixels() const;
    const unsigned char* getPixelData() const;

    void addWalls();

    void PrintPicture();

private:
    uint64_t N1;
    uint64_t N2;
    uint64_t N3;

    double resolution;
    unsigned char* pixels;

    bool wallsAdded;

    Picture(const Picture&);
    Picture& operator=(const Picture&);
};

#endif
