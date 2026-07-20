#include "GameEngine.hpp"
#include "RuleEngine.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (gameOver_) return false;

    Position from{fromRow, fromCol};
    Position to{toRow, toCol};

    if (arbiter_.hasActiveMotionFrom(from)) return false;
    if (arbiter_.conflictsWithActiveMotion(from, to)) return false;

    bool valid = RuleEngine::validateMove(board_, fromRow, fromCol, toRow, toCol);
    if (!valid) return false;

    auto piece = board_.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;

    int distance = std::max(std::abs(toRow - fromRow), std::abs(toCol - fromCol));

    const auto& moveCfg = configs_.get(piece->color(), piece->type(), PieceState::MOVE);
    long durationMs = (moveCfg.speedMetersPerSec > 0.0)
        ? static_cast<long>((distance / moveCfg.speedMetersPerSec) * 1000.0)
        : distance * 100L;
    if (durationMs < 1) durationMs = 1;   // מונע 0/0 (NaN) בהמשך

    Motion motion{
    motion.from = from,
    motion.to = to,
    motion.piece = *piece,
    motion.state = PieceState::MOVE,
    motion.startTime = 0,
    motion.durationMs = durationMs};

    arbiter_.startMotion(motion);
    return true;
}

bool GameEngine::requestJump(int row, int col) {
    if (gameOver_) return false;

    Position pos{row, col};

    if (arbiter_.hasActiveMotionFrom(pos)) return false;
    if (arbiter_.isJumpingAt(pos)) return false;

    auto piece = board_.getCell(row, col);
    if (!piece.has_value()) return false;

    const auto& jumpCfg = configs_.get(piece->color(), piece->type(), PieceState::JUMP);
    long durationMs = (jumpCfg.framesPerSecond > 0 && jumpCfg.frameCount > 0)
        ? static_cast<long>(jumpCfg.frameCount) * 1000L / jumpCfg.framesPerSecond
        : 100L;
    if (durationMs < 1) durationMs = 1;   // מונע 0/0 (NaN) בהמשך

    arbiter_.startJump(pos, *piece, durationMs);
    return true;
}

void GameEngine::update(long ms)
{
    auto captured = arbiter_.advanceTime(ms, board_);

    for (const auto& piece : captured)
    {
        if (piece.type() == Piece::Type::KING)
        {
            gameOver_ = true;
            break;
        }
    }
}
std::optional<RenderPosition>
GameEngine::currentPositionOf(const Position& from) const {
    return arbiter_.currentPositionOf(from);
}

long GameEngine::currentTime() const { return arbiter_.currentTime(); }

std::optional<PieceState> GameEngine::currentStateOf(const Position& from) const
{
    return arbiter_.currentStateOf(from);
}