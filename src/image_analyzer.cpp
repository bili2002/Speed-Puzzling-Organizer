#include <math.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "image_analyzer.h"

using Coordinates = ImageAnalyzer::Coordinates;
using Pixel = ImageAnalyzer::Pixel;
using Pixels = ImageAnalyzer::Pixels;
using Zones = ImageAnalyzer::Zones;
using PieceDifficulty = ImageAnalyzer::PieceDifficulty;

const std::vector<Coordinates> ImageAnalyzer::directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

double ImageAnalyzer::difficultyToTime(double difficulty) {
    return exp(DIFFICULTY_COEFFICIENT * difficulty);
}

std::ostream& operator<<(std::ostream& stream, const Pixel& pixel) {
    stream << pixel.r << ' ' << pixel.g << ' ' << pixel.b;
    return stream;
}

std::istream& operator>>(std::istream& stream, Pixel& pixel) {
    stream >> pixel.r >> pixel.g >> pixel.b;
    return stream;
}

Pixel& Pixel::operator/=(int divBy) {
    r /= divBy;
    g /= divBy;
    b /= divBy;

    return *this;
} 

Pixel& Pixel::operator+=(const Pixel& other) {
    r += other.r;
    g += other.g;
    b += other.b;

    return *this;
} 

bool Pixel::operator==(const Pixel& other) const {
    return r == other.r && g == other.g && b == other.b;
} 

int Pixel::distance(const Pixel& other) const {
    auto oneColorDistance = [](int left, int right) {
        return (left - right) * (left - right);
    };

    return oneColorDistance(r, other.r) + oneColorDistance(b, other.b) + oneColorDistance(g, other.g);
}

Coordinates Coordinates::operator+(const Coordinates& other) const {
    return {x + other.x, y + other.y};
}

bool Coordinates::inBound(int height, int width) const {
    return x >= 0 && x < height && y >= 0 && y < width;
}

template<typename T>
inline void ImageAnalyzer::setVectorDimensions(std::vector<std::vector<T>>& vec, int height, int width) const {
    vec.resize(height);
    for (auto& row : vec) {
        row.resize(width);
    }
}   

template<typename T>
inline int ImageAnalyzer::width(const std::vector<std::vector<T>>& vec) {
    return vec[0].size();
}   

template<typename T>
inline int ImageAnalyzer::height(const std::vector<std::vector<T>>& vec) {
    return vec.size();
}   

void ImageAnalyzer::findRatioForPieces() {
    ratioPixelsToPiece = std::sqrt((double)imageWidth * imageHeight / puzzlePieces);
}

Coordinates ImageAnalyzer::fromPixelToPiece(const Coordinates& pixelCoordinates) const {
    return {(int)round(pixelCoordinates.x / ratioPixelsToPiece), (int)round(pixelCoordinates.y / ratioPixelsToPiece)};
}

Pixels ImageAnalyzer::flattenColors(const Pixels& pixels, int maxColorValue) const {
    int divBy = MAX_COLOR_VALUE / maxColorValue;

    Pixels newColors = pixels;
    for (auto& row : newColors) {
        for (auto& cell : row) {
            cell /= divBy;
        }
    }

    return newColors;
}

void ImageAnalyzer::findCurrentZone(Coordinates curr, Zones& zones, const Pixels& pixels, int zoneNumber) const {
    if (zones[curr.x][curr.y] != 0) {
        return;
    }
    zones[curr.x][curr.y] = zoneNumber;

    for (auto& direction : directions) {
        auto neighbour = curr + direction;

        if (!neighbour.inBound(height(pixels), width(pixels))) {
            continue;
        }

        if (pixels[neighbour.x][neighbour.y] == pixels[curr.x][curr.y]) {
            findCurrentZone(neighbour, zones, pixels, zoneNumber);
        }
    }
}

std::tuple<Zones, int> ImageAnalyzer::getZones(const Pixels& pixels) const {
    auto flattenedPixels = flattenColors(pixels, ZONES_MAX_COLOR_VALUE);
    Zones zones;

    setVectorDimensions(zones, height(pixels), width(pixels));

    int nextZone = 1;
    for (int i = 0; i < height(pixels); i++) {
        for (int j = 0; j < width(pixels); j++) {
            if (zones[i][j] == 0) {
                findCurrentZone({i, j}, zones, flattenedPixels, nextZone);
                nextZone++;
            }            
        }
    }

    return {zones, nextZone - 1};
}

Pixels ImageAnalyzer::getPiecesAverageColor() const {
    Pixels piecesColor;
    std::vector<std::vector<int>> pixelsInPiece;

    auto [puzzleHeight, puzzleWidth] = fromPixelToPiece({imageHeight, imageWidth});
    puzzleHeight++, puzzleWidth++;

    setVectorDimensions(piecesColor, puzzleHeight, puzzleWidth);
    setVectorDimensions(pixelsInPiece, puzzleHeight, puzzleWidth);

    for (int i = 0; i < imageHeight; i++) {
        for (int j = 0; j < imageWidth; j++) {
            auto [x, y] = fromPixelToPiece({i, j});
            
            piecesColor[x][y] += imagePixels[i][j];
            pixelsInPiece[x][y]++;
        }
    }

    for (int i = 0; i < puzzleHeight; i++) {
        for (int j = 0; j < puzzleWidth; j++) {
            piecesColor[i][j] /= pixelsInPiece[i][j];
        }
    }

    return piecesColor;
}  

PieceDifficulty ImageAnalyzer::findGradientInZones() const {
    auto piecesColor = getPiecesAverageColor();
    auto zones = getZones(piecesColor);

    PieceDifficulty pieceDifficulty;
    setVectorDimensions(pieceDifficulty, height(piecesColor), width(piecesColor));

    int divBy = MAX_COLOR_VALUE / ZONES_MAX_COLOR_VALUE;
    int maxDistance = 3 * divBy * divBy;

    for (int i = 0; i < height(piecesColor); i++) {
        for (int j = 0; j < width(piecesColor); j++) {
            int countDistance = 0;
            int totalDistance = 0;

            for (auto& direction : directions) {
                auto neighbour = Coordinates({i, j}) + direction;

                if (!neighbour.inBound(height(piecesColor), width(piecesColor))) {
                    continue;
                }
                
                countDistance++;
                totalDistance += piecesColor[i][j].distance(piecesColor[neighbour.x][neighbour.y]);
            }

            double averageDistance = countDistance > 0 ? totalDistance / countDistance : 1;

            pieceDifficulty[i][j] = 1 - averageDistance / maxDistance;
        }
    }

    return pieceDifficulty;
}

PieceDifficulty ImageAnalyzer::getTotalDifficulty() const {
    PieceDifficulty pieceDifficulty = findGradientInZones();
    return pieceDifficulty;
}

void ImageAnalyzer::writeToFile(const Pixels& pixels, std::string fileName) const {
    std::ofstream file(fileName);
    
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open file");
    }

    for (auto& row : pixels) {
        for (auto& cell : row) {
            file << cell << '\n';
        }
    }
} 

ImageAnalyzer::ImageAnalyzer(const std::string& imageFile, int pieces) : puzzlePieces(pieces) {
    std::ifstream file(imageFile);

    std::string temp;
    file >> temp >> imageWidth >> imageHeight;
    file >> temp;

    setVectorDimensions(imagePixels, imageHeight, imageWidth);

    for (int i = 0; i < imageHeight; i++) {
        for (int j = 0; j < imageWidth; j++) {
            file >> imagePixels[i][j];
        }
    }

    findRatioForPieces();
}

double ImageAnalyzer::getDifficulty() const {
    PieceDifficulty pieceDifficulty = getTotalDifficulty();

    double totalSum = 0;
    for (auto& row : pieceDifficulty) {
        for (auto& cell : row) {
            totalSum += difficultyToTime(cell);
        }
    }

    std::cout << totalSum << std::endl;
    return totalSum;
}