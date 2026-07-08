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

void Board::print(std::ostream& out) const {
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

std::optional<Position> ClickTranslator::pixelToCell(int x, int y, const Board& board) {
    if (x < 0 || y < 0) return std::nullopt;

    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;

    if (row < 0 || row >= board.rows() || col < 0 || col >= board.cols()) {
        return std::nullopt;
    }
    return Position{row, col};
}
bool Board::isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) const
{
    std::optional<Piece> piece = getCell(fromRow, fromCol);

    // אין כלי במקום שממנו מנסים לזוז
    if (!piece.has_value())
        return false;


    // בדיקת צורת התנועה לפי סוג הכלי
    if (!piece->isValidShape(
            fromRow,
            fromCol,
            toRow,
            toCol))
    {
        return false;
    }


    // בדיקת כלי ביעד
    std::optional<Piece> destination = getCell(toRow, toCol);

    // אם יש כלי באותו צבע - אסור לאכול אותו
    if (destination.has_value())
    {
        if (destination->color() == piece->color())
            return false;
    }


    // בדיקת חסימות בדרך
    auto path = piece->getPath(
        fromRow,
        fromCol,
        toRow,
        toCol
    );


    for (auto pos : path)
    {
        if (getCell(pos.first, pos.second).has_value())
            return false;
    }


    return true;
}