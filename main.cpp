#include <string>
#include "src/image_analyzer.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        throw std::invalid_argument("Use as ./main.cpp {puzzle_name} {number_of_pieces}");
    }

    auto imageAnalyzer = ImageAnalyzer(argv[1], stoi(argv[2]));

}