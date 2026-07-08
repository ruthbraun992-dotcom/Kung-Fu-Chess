#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <optional>
#include <ostream>
#include "Piece.hpp"   // Board contains Piece objects, so it needs the full definition

// A single board cell address (row, col), zero-based.
struct Position {
    int row;
    int col;

    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

class Board {
public:
    Board(int rows, int cols);

    void setCell(int row, int col, std::optional<Piece> piece);
    std::optional<Piece> getCell(int row, int col) const;

    int rows() const;
    int cols() const;

    // Prints the board in canonical form: tokens separated by single
    // spaces, one row per line.
    void print(std::ostream& out) const;
    bool isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol  
    ) const;

private:
    int rows_;
    int cols_;
    std::vector<std::vector<std::optional<Piece>>> cells_;
};

// Converts pixel coordinates into a board Position.
// Each cell is 100x100 pixels. Returns std::nullopt if the click
// lands outside the board.
class ClickTranslator {
public:
    static constexpr int CELL_SIZE = 100;

    static std::optional<Position> pixelToCell(int x, int y, const Board& board);
};

#endif // BOARD_H