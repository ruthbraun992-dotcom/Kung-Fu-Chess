// RuleEngine.hpp
#pragma once
#include "Board.hpp"


namespace RuleEngine {
    bool validateMove(const Board& board, int fromRow, int fromCol, int toRow, int toCol);
}