// PieceRules.cpp
#include "PieceRules.hpp"
#include <cmath>

namespace PieceRules {

bool isValidShape(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol) {
    int rowDiff = std::abs(toRow - fromRow);
    int colDiff = std::abs(toCol - fromCol);

    switch (piece.type()) {
        case Piece::Type::KING:
            return rowDiff <= 1 && colDiff <= 1 && (rowDiff != 0 || colDiff != 0);

        case Piece::Type::ROOK:
            return fromRow == toRow || fromCol == toCol;

        case Piece::Type::BISHOP:
            return rowDiff == colDiff;

        case Piece::Type::QUEEN:
            return (fromRow == toRow || fromCol == toCol) || rowDiff == colDiff;

        case Piece::Type::KNIGHT:
            return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);

        case Piece::Type::PAWN: {
            int rowStep = toRow - fromRow;
            int colStep = std::abs(toCol - fromCol);
            int forward = (piece.color() == Piece::Color::WHITE) ? -1 : 1;
            return rowStep == forward && colStep <= 1;
        }
    }
    return false;
}

std::vector<std::pair<int,int>> getPath(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol) {
    std::vector<std::pair<int,int>> path;
    int rowStep = 0, colStep = 0;

    switch (piece.type()) {
        case Piece::Type::ROOK:
        case Piece::Type::QUEEN:
            if (fromRow == toRow) colStep = (toCol > fromCol) ? 1 : -1;
            else if (fromCol == toCol) rowStep = (toRow > fromRow) ? 1 : -1;
            break;
        case Piece::Type::BISHOP:
            rowStep = (toRow > fromRow) ? 1 : -1;
            colStep = (toCol > fromCol) ? 1 : -1;
            break;
        default:
            return path; // KING, KNIGHT, PAWN: no intermediate cells
    }

    int row = fromRow + rowStep, col = fromCol + colStep;
    while (row != toRow || col != toCol) {
        path.push_back({row, col});
        row += rowStep;
        col += colStep;
    }
    return path;
}

} // namespace PieceRules