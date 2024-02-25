#include <exception>
#include <string>
#include <vector>

class ImageAnalyzer {
public:
    struct Pixel {
        int r;
        int g;
        int b;

        Pixel& operator/=(int by);
        // friend std::ostream& operator<<(std::ostream stream, const Pixel& pixel);
        // friend std::istream& operator>>(std::istream& stream, ImageAnalyzer::Pixel& pixel);
    };

    struct Coordinates {
        int x;
        int y;
    };
    

    using Pixels = std::vector<std::vector<Pixel>>;
    using Zones = std::vector<std::vector<int>>;
    using PieceDifficulty = std::vector<std::vector<double>>;

private:
    static const int MaxColorValue = 256;
    static const int MainColors = 3;

    Pixels pixels;
    int pieces;
    int height;
    int width;

    float ratio;

    void findRatioForPieces();    
    Coordinates fromPixelToPiece(const Coordinates& pixelCoordinates) const;

    Pixels flattenColors(int numberOfTotalColors) const;
    Zones getZones() const;
    PieceDifficulty countZones() const;

    Pixels getGrey() const;
    PieceDifficulty countDistingushableElements() const;
    PieceDifficulty findPatterns() const;

    PieceDifficulty findGradientInZone(const Zones& zone) const;

    PieceDifficulty getTotalDifficulty() const;

    void writeToFile(const Pixels& pixels, std::string fileName) const; // for debbuging 

public:
    ImageAnalyzer() = delete;
    ImageAnalyzer(const std::string& imageFile, int pieces);

    double getDifficulty() const;
};
