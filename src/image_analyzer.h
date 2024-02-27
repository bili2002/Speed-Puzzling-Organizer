#include <exception>
#include <string>
#include <vector>

class ImageAnalyzer {
public:
    struct Pixel {
        int r;
        int g;
        int b;

        Pixel& operator/=(int divBy);
        Pixel& operator+=(const Pixel& other);
        
        bool operator==(const Pixel& other) const;
        // friend std::ostream& operator<<(std::ostream stream, const Pixel& pixel);
        // friend std::istream& operator>>(std::istream& stream, ImageAnalyzer::Pixel& pixel);

        int distance(const Pixel& other) const;
    };

    struct Coordinates {
        int x;
        int y;

        Coordinates operator+(const Coordinates& other) const;

        bool inBound(int height, int width) const;
    };
    

    using Pixels = std::vector<std::vector<Pixel>>;
    using Zones = std::vector<std::vector<int>>;
    using PieceDifficulty = std::vector<std::vector<double>>;

private:
    static const int MAX_COLOR_VALUE = 255;
    static const int MAIN_COLORS = 3;

    static const int ZONES_MAX_COLOR_VALUE = 10;
    static const int DIFFICULTY_COEFFICIENT = 3;

    static const std::vector<Coordinates> directions;

    static double difficultyToTime(double difficulty);

    Pixels imagePixels;
    int puzzlePieces;
    int imageHeight;
    int imageWidth;

    float ratioPixelsToPiece;

    template<typename T>
    inline void setVectorDimensions(std::vector<std::vector<T>>& vec, int height, int width) const;

    template<typename T>
    static inline int width(const std::vector<std::vector<T>>& vec);

    template<typename T>
    static inline int height(const std::vector<std::vector<T>>& vec);

    void findRatioForPieces();    
    Coordinates fromPixelToPiece(const Coordinates& pixelCoordinates) const;

    Pixels flattenColors(const Pixels& pixels, int maxColorValue) const;
    void findCurrentZone(Coordinates curr, Zones& zones, const Pixels& pixels, int zoneNumber) const;
    std::tuple<Zones, int> getZones(const Pixels& pixels) const;
    // PieceDifficulty countZones() const;

    Pixels getPiecesAverageColor() const;

    Pixels getGrey() const;
    PieceDifficulty countDistingushableElements() const;
    PieceDifficulty findPatterns() const;

    PieceDifficulty findGradientInZones() const;

    PieceDifficulty getTotalDifficulty() const;

    void writeToFile(const Pixels& pixels, std::string fileName) const; // for debbuging 

public:
    ImageAnalyzer() = delete;
    ImageAnalyzer(const std::string& imageFile, int pieces);

    double getDifficulty() const;
};
