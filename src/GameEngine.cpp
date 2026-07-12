#include "GameEngine.hpp"
#include "RuleEngine.hpp"
#include <cmath>
#include <algorithm>

bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (gameOver_) return false; // אחרי סיום המשחק - כל פקודה מתעלמת

    Position from{fromRow, fromCol};
    Position to{toRow, toCol};

    if (arbiter_.hasActiveMotionFrom(from)) return false;
    if (arbiter_.conflictsWithActiveMotion(from, to)) return false;
    if (!RuleEngine::validateMove(board_, fromRow, fromCol, toRow, toCol)) return false;

    auto piece = board_.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;

    int distance = std::max(std::abs(toRow - fromRow), std::abs(toCol - fromCol));
    arbiter_.startMotion(Motion{from, to, *piece, distance * 1000L});
    return true;
}

void GameEngine::wait(long ms) {
    if (gameOver_) return; // אין טעם לקדם זמן אחרי סיום

    auto captured = arbiter_.advanceTime(ms, board_);
    for (const auto& piece : captured) {
        if (piece.type() == Piece::Type::KING) {
            gameOver_ = true;
            break;
        }
    }
}