#pragma once
#include "Piece.hpp"
#include "model/Board.hpp"

#include <vector>
#include <utility>

namespace PieceRules {
    bool isValidShape(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol, int boardRows);
    std::vector<std::pair<int,int>> getPath(const Piece& piece, int fromRow, int fromCol, int toRow, int toCol);
}