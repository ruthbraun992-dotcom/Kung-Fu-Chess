#include "GameEngine.hpp"
#include "rules/RuleEngine.hpp"
#include "events/Events.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {
        std::cout << "ENGINE MOVE START1" << std::endl;
if (gameOver_) return false;
        std::cout << "ENGINE MOVE START2" << std::endl;

    Position from{fromRow, fromCol};
    Position to{toRow, toCol};
        std::cout << "ENGINE MOVE START3" << std::endl;

    if (arbiter_.hasActiveMotionFrom(from)) return false;
    if (arbiter_.conflictsWithActiveMotion(from, to)) return false;
        std::cout << "ENGINE MOVE START4" << std::endl;

    bool valid = RuleEngine::validateMove(board_, fromRow, fromCol, toRow, toCol);
    if (!valid) return false;
        std::cout << "ENGINE MOVE START5" << std::endl;

    auto piece = board_.getCell(fromRow, fromCol);
    if (!piece.has_value()) return false;
        std::cout << "ENGINE MOVE START6" << std::endl;

    int distance = std::max(std::abs(toRow - fromRow), std::abs(toCol - fromCol));
        std::cout << "ENGINE MOVE START7" << std::endl;
std::cout 
    << "piece color="
    << static_cast<int>(piece->color())
    << " type="
    << static_cast<int>(piece->type())
    << std::endl;
    const auto& moveCfg = configs_.get(piece->color(), piece->type(), PieceState::MOVE);
    long durationMs = (moveCfg.speedMetersPerSec > 0.0)
        ? static_cast<long>((distance / moveCfg.speedMetersPerSec) * 1000.0)
        : distance * 100L;
    if (durationMs < 1) durationMs = 1;   // מונע 0/0 (NaN) בהמשך
        std::cout << "ENGINE MOVE START8" << std::endl;

    Motion motion{
    motion.from = from,
    motion.to = to,
    motion.piece = *piece,
    motion.state = PieceState::MOVE,
    motion.startTime = 0,
    motion.durationMs = durationMs};
        std::cout << "ENGINE MOVE START9" << std::endl;

    arbiter_.startMotion(motion);
    stats_.recordMove(piece->color(), piece->type(), from, to, /*isJump=*/false, arbiter_.currentTime());
    bus_.publish(MoveLoggedEvent{piece->color(), "Move to "+ to.toString()});
    bus_.publish(SoundEvent{SoundType::Move});
        std::cout << "ENGINE MOVE END10" << std::endl;

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
    stats_.recordMove(piece->color(), piece->type(), pos, pos, /*isJump=*/true, arbiter_.currentTime());
    bus_.publish(MoveLoggedEvent{piece->color(), "Jump to " + pos.toString()});
    bus_.publish(SoundEvent{SoundType::Move});
    return true;
}

void GameEngine::update(long ms)
{
    auto captured = arbiter_.advanceTime(ms, board_);

    for (const auto& c : captured)
    {
        stats_.recordCapture(c);

        bus_.publish(PieceCapturedEvent{
            c.capturedPiece.color(),
            c.capturedPiece.type(),
            c.at
        });

        bus_.publish(ScoreUpdatedEvent{
            c.capturer.color(),
            stats_.score(c.capturedPiece.color())
        });

        bus_.publish(SoundEvent{SoundType::Capture});

        if (c.capturedPiece.type() == Piece::Type::KING)
        {
            gameOver_ = true;
            bus_.publish(GameOverEvent{
                c.capturer.color(),
                "king_captured"
            });
        }
    }
}

std::optional<RenderPosition> GameEngine::currentPositionOf(const Position& from) const {
    return arbiter_.currentPositionOf(from);
}

long GameEngine::currentTime() const { return arbiter_.currentTime(); }

std::optional<PieceState> GameEngine::currentStateOf(const Position& from) const
{
    return arbiter_.currentStateOf(from);
}

std::optional<long> GameEngine::stateStartTimeOf(const Position& from) const
{
    return arbiter_.stateStartTimeOf(from);
}

std::optional<long> GameEngine::stateDurationOf(const Position& from) const
{
    return arbiter_.stateDurationOf(from);
}