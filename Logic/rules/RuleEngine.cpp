// RuleEngine.cpp
#include "RuleEngine.hpp"
#include "PieceRules.hpp"
#include <cmath>

namespace RuleEngine {

bool validateMove(const Board& board, int fromRow, int fromCol, int toRow, int toCol) {
   if (fromRow == toRow && fromCol == toCol) return false; 
      std::optional<Piece> piece = board.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;
    
    if (!PieceRules::isValidShape(*piece, fromRow, fromCol, toRow, toCol, board.rows())) return false;

    std::optional<Piece> destination = board.getCell(toRow, toCol);

    if (destination.has_value() && destination->color() == piece->color()) {
        return false; 
    }

    if (piece->type() == Piece::Type::PAWN) {
        int colDiff = std::abs(toCol - fromCol);
        if (colDiff == 0 && destination.has_value()) return false;
        if (colDiff == 1 && !destination.has_value()) return false; 
    }

    for (auto pos : PieceRules::getPath(*piece, fromRow, fromCol, toRow, toCol)) {
        if (board.getCell(pos.first, pos.second).has_value()) return false;
    }

    return true;
}

} // namespace RuleEngine