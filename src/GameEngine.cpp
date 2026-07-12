// GameEngine.cpp
#include "GameEngine.hpp"
#include "RuleEngine.hpp"
#include <cmath>
#include <algorithm>
  
bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {
    Position from{fromRow, fromCol};
    Position to{toRow, toCol};

    if (arbiter_.hasActiveMotionFrom(from)) return false;        // invalid premove
    if (arbiter_.conflictsWithActiveMotion(from, to)) return false; // movement conflict

    if (!RuleEngine::validateMove(board_, fromRow, fromCol, toRow, toCol)) return false;

    auto piece = board_.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;

    int distance = std::max(std::abs(toRow - fromRow), std::abs(toCol - fromCol));

    arbiter_.startMotion(Motion{from, to, *piece, distance * 1000L});
    return true;
}