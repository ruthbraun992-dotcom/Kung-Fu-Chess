#include "GameEngine.hpp"
#include "RuleEngine.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>


bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (gameOver_) return false; // אחרי סיום המשחק - כל פקודה מתעלמת

    Position from{fromRow, fromCol};
    Position to{toRow, toCol};

    if (arbiter_.hasActiveMotionFrom(from)) return false;
    if (arbiter_.conflictsWithActiveMotion(from, to)) return false;
    bool valid = RuleEngine::validateMove(
        board_,
        fromRow,
        fromCol,
        toRow,
        toCol
    );

std::cout << "Rule valid = "
          << valid
          << std::endl;

    if (!valid)    return false;
    auto piece = board_.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;

    int distance = std::max(std::abs(toRow - fromRow), std::abs(toCol - fromCol));
    Motion motion {
    motion.from = from,
    motion.to = to,
    motion.piece = *piece,
    motion.startTime=0,
    motion.durationMs = distance * 100L};

    arbiter_.startMotion(motion);
    return true;
}

bool GameEngine::requestJump(int row, int col) {
    if (gameOver_) return false;


    Position pos{row, col};

    if (arbiter_.hasActiveMotionFrom(pos)) return false; // כלל 4: כלי בתנועה לא יכול לקפוץ
    if (arbiter_.isJumpingAt(pos)) return false;          // כלי כבר קופץ - לא כפל-קפיצה

    auto piece = board_.getCell(row, col);
    if (!piece.has_value()) return false; // כלל 5: אין כלי = לא ניתן לקפוץ (כולל כלי שנתפס)

    arbiter_.startJump(pos, *piece, 100L);
    return true;
}
void GameEngine::update(long ms)
{
    arbiter_.advanceTime(ms, board_);
}
std::optional<RenderPosition>
GameEngine::currentPositionOf(const Position& from) const
{
    return arbiter_.currentPositionOf(from);
}


long GameEngine::currentTime() const
{
    return arbiter_.currentTime();
}


