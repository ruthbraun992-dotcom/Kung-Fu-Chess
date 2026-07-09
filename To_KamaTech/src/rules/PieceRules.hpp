// PieceRules.hpp
#pragma once
#include "model/Piece.hpp"
#include "model/Board.hpp"

#include <vector>
#include <utility>

namespace PieceRules {
    bool isValidShape(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol);
    std::vector<std::pair<int,int>> getPath(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol);
}