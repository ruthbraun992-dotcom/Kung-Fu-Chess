#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <optional>
#include <ostream>
#include "position.hpp"
#include "Piece.hpp"

class Board {

public:
    Board(int rows, int cols);

    void setCell(int row, int col, std::optional<Piece> piece);
    std::optional<Piece> getCell(int row, int col) const;

    int rows() const;
    int cols() const;

    void Printer(std::ostream& out) const;
    bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;

private:
    int rows_;
    int cols_;
    std::vector<std::vector<std::optional<Piece>>> cells_;
};


#endif // BOARD_H