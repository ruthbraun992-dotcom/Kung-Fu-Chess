#pragma once
#include <string>
#include <tuple>

struct Position {
    int row;
    int col;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
     
    bool operator<(const Position& other) const {
        return std::tie(row, col) < std::tie(other.row, other.col);
    }

    std::string toString() const {
        return "(" + std::to_string(row) + "," + std::to_string(col) + ")";
    }
};