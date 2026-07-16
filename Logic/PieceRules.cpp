#include "PieceRules.hpp"
#include <cmath>

namespace PieceRules {

bool isValidShape(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol, int boardRows) {
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
            int startRow = (piece.color() == Piece::Color::WHITE) ? boardRows - 2 : 1;

            if (colStep == 0) {
                if (rowStep == forward) return true;
                if (rowStep == 2 * forward && fromRow == startRow) return true;
                return false;
            }
            return rowStep == forward && colStep == 1;
        }
    }
    return false;
}
std::vector<std::pair<int,int>> getPath(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol) {
    std::vector<std::pair<int,int>> path;
    if (fromRow == toRow && fromCol == toCol) return path;

    int rowStep = 0, colStep = 0;

    switch (piece.type()) {
        case Piece::Type::ROOK:
            if (fromRow == toRow) colStep = (toCol > fromCol) ? 1 : -1;
            else if (fromCol == toCol) rowStep = (toRow > fromRow) ? 1 : -1;
            break;

        case Piece::Type::BISHOP:
            rowStep = (toRow > fromRow) ? 1 : -1;
            colStep = (toCol > fromCol) ? 1 : -1;
            break;

        case Piece::Type::QUEEN:
            if (fromRow == toRow) {
                colStep = (toCol > fromCol) ? 1 : -1;
            } else if (fromCol == toCol) {
                rowStep = (toRow > fromRow) ? 1 : -1;
            } else {
                rowStep = (toRow > fromRow) ? 1 : -1;
                colStep = (toCol > fromCol) ? 1 : -1;
            }
            break;

        case Piece::Type::PAWN: {
            int rowDiff = std::abs(toRow - fromRow);
            if (rowDiff == 2 && fromCol == toCol) {
                int midRow = (fromRow + toRow) / 2;
                path.push_back({midRow, fromCol});
            }
            return path;
        }
        default:
            return path; // KING, KNIGHT: no intermediate cells
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