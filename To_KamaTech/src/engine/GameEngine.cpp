// GameEngine.cpp
#include "GameEngine.hpp"

#include "rules/RuleEngine.hpp"
#include <cmath>
#include <algorithm>

bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (arbiter_.hasActiveMotion()) return false; // "motion_in_progress"

    if (!RuleEngine::validateMove(board_, fromRow, fromCol, toRow, toCol)) return false;

    auto piece = board_.getCell(fromRow, fromCol);
    int distance = std::max(std::abs(toRow - fromRow), std::abs(toCol - fromCol));

    arbiter_.startMotion(Motion{
        Position{fromRow, fromCol},
        Position{toRow, toCol},
        *piece,
        distance * 1000L
    });
    return true;
}