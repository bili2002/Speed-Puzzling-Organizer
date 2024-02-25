#include <iostream>
#include <exception>
#include <string>
#include <vector>

class ImageAnalyzer {
    struct Pixel {
        char r;
        char g;
        char b;
    };

    struct Coordinates {
        int x;
        int y;
    };

    using Pixels = std::vector<std::vector<Pixel>>;
    using Zones = std::vector<std::vector<int>>;
    using PieceDifficulty = std::vector<std::vector<double>>;

private:
    PieceDifficulty pieceDifficulty;
    Pixels pixels;
    int height;
    int width;

    Coordinates fromPixelToPiece(const Coordinates& pixelCoordinates);
    void findSize();    

    Pixels flattenColors(int by);
    Zones getZones();
    PieceDifficulty countZones();

    Pixels getGrey();
    PieceDifficulty countDistingushableElements();
    PieceDifficulty findPatterns();

    PieceDifficulty findGradientInZone(const Zones& zone);

    PieceDifficulty getTotalDifficulty();
    
public:
    ImageAnalyzer() = delete;
    ImageAnalyzer(const std::string& image_file, int pieces);

    double getDifficulty();
};
