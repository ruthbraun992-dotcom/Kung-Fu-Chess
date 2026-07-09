#include "Board.hpp"


Board::Board(int rows, int cols)
    : rows_(rows), cols_(cols),
      cells_(rows, std::vector<std::optional<Piece>>(cols, std::nullopt)) {}

void Board::setCell(int row, int col, std::optional<Piece> piece) {
    cells_[row][col] = piece;
}

std::optional<Piece> Board::getCell(int row, int col) const {
    return cells_[row][col];
}

int Board::rows() const { return rows_; }
int Board::cols() const { return cols_; }

void Board::Printer(std::ostream& out) const {
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (c > 0) out << ' ';
            if (cells_[r][c].has_value()) {
                out << cells_[r][c]->toString();
            } else {
                out << '.';
            }
        }
        out << '\n';
    }
}


