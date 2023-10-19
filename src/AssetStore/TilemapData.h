#pragma once

#include <string>

struct TilemapData {
    int tileSize;
    double tileScale;
    int numCols;
    int numRows;
    std::string filePath;

    TilemapData(
        int tileSize,
        double tileScale,
        int numCols,
        int numRows,
        std::string filePath
    ) {
        this->tileSize = tileSize;
        this->tileScale = tileScale;
        this->numCols = numCols;
        this->numRows = numRows;
        this->filePath = filePath;
    }


};