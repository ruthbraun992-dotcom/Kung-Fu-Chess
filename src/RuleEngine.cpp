// RuleEngine.cpp
#include "RuleEngine.hpp"
#include "PieceRules.hpp"
#include <cmath>

namespace RuleEngine {

bool validateMove(const Board& board, int fromRow, int fromCol, int toRow, int toCol) {
    std::optional<Piece> piece = board.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;

    if (!PieceRules::isValidShape(*piece, fromRow, fromCol, toRow, toCol)) return false;

    std::optional<Piece> destination = board.getCell(toRow, toCol);

    if (destination.has_value() && destination->color() == piece->color()) {
        return false; // cannot capture friendly piece
    }

    // Pawn forward move must land on an empty cell (no straight capture)
    if (piece->type() == Piece::Type::PAWN) {
        int colDiff = std::abs(toCol - fromCol);
        if (colDiff == 0 && destination.has_value()) return false;
        if (colDiff == 1 && !destination.has_value()) return false; // diagonal must capture
    }

    for (auto pos : PieceRules::getPath(*piece, fromRow, fromCol, toRow, toCol)) {
        if (board.getCell(pos.first, pos.second).has_value()) return false;
    }

    return true;
}

} // namespace RuleEngine