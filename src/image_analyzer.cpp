#include <math.h>
#include <fstream>
#include <iostream>
#include "image_analyzer.h"

std::ostream& operator<<(std::ostream& stream, const ImageAnalyzer::Pixel& pixel) {
    stream << pixel.r << ' ' << pixel.g << ' ' << pixel.b;
    return stream;
}

std::istream& operator>>(std::istream& stream, ImageAnalyzer::Pixel& pixel) {
    stream >> pixel.r >> pixel.g >> pixel.b;
    return stream;
}

ImageAnalyzer::Pixel& ImageAnalyzer::Pixel::operator/=(int by) {
    r /= by;
    g /= by;
    b /= by;

    return *this;
} 

void ImageAnalyzer::findRatioForPieces() {
    ratio = ((double)width * height / pieces);
}

ImageAnalyzer::Coordinates ImageAnalyzer::fromPixelToPiece(const Coordinates& pixelCoordinates) const {
    return {(int)round(pixelCoordinates.x / ratio), (int)round(pixelCoordinates.y / ratio)};
}

ImageAnalyzer::Pixels ImageAnalyzer::flattenColors(int numberOfTotalColors) const {
    int by = MaxColorValue * MainColors / numberOfTotalColors;

    ImageAnalyzer::Pixels newColors = pixels;
    for (auto& row : newColors) {
        for (auto& cell : row) {
            cell /= by;
        }
    }

    return newColors;
}

// Zones ImageAnalyzer::getZones() const;
// PieceDifficulty ImageAnalyzer::countZones();

// Pixels getGrey();
// PieceDifficulty countDistingushableElements();
// PieceDifficulty findPatterns();

// PieceDifficulty findGradientInZone(const Zones& zone);

// PieceDifficulty getTotalDifficulty() const;

void ImageAnalyzer::writeToFile(const Pixels& pixels, std::string fileName) const {
    std::ofstream file(fileName);

    Pixel tt({1, 2, 3});
    file << tt;

    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open file");
    }

    for (auto& row : pixels) {
        for (auto& cell : row) {
            file << cell << '\n';
        }
    }
} 

ImageAnalyzer::ImageAnalyzer(const std::string& imageFile, int pieces) : pieces(pieces) {
    std::ifstream file(imageFile);

    std::string temp;
    file >> temp >> width >> height;
    file >> temp;

    pixels.resize(height);
    for (int i = 0; i < height; i++) {
        pixels[i].resize(width);
        for (int j = 0; j < width; j++) {
            file >> pixels[i][j];
        }
    }
}

double ImageAnalyzer::getDifficulty() const {
    //testing here for now
    auto temp = flattenColors(4 * 3);
    writeToFile(temp, "temp.ppm");

    return 0;
}