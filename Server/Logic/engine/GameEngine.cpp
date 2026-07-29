#include "GameEngine.hpp"
#include "rules/RuleEngine.hpp"
#include "events/Events.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool GameEngine::requestMove(int fromRow, int fromCol, int toRow, int toCol) {

std::cout << "REQUEST MOVE CALLED\n";
    if (gameOver_) return false;

    Position from{fromRow, fromCol};
    Position to{toRow, toCol};

    if (arbiter_.hasActiveMotionFrom(from)) return false;
if (arbiter_.hasActiveMotionTo(to)) return false;
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
    from,
    to,
   *piece,
   PieceState::MOVE,
   0,
   durationMs};

startMotion(motion);
std::cout << "MOVE STARTED" << std::endl;

if (isGameOver())
{
    std::cout << "🎯 DETECTED GAME OVER IN REQUEST MOVE" << std::endl;
}
    stats_.recordMove(piece->color(), piece->type(), from, to, /*isJump=*/false, arbiter_.currentTime());
    bus_.publish(MoveLoggedEvent{piece->color(), "Move to " + to.toString()});
bus_.publish(SoundEvent{SoundType::Move});
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
    
 for (const auto c : captured)
    {
        std::cout << "[SERVER] CAPTURED: " << (c.capturedPiece.color() == Piece::Color::WHITE ? "WHITE" : "BLACK")
                  << " " << static_cast<int>(c.capturedPiece.type()) << std::endl;
    

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
            std::cout << "🏁 KING CAPTURED! Publishing GameOverEvent" << std::endl;
        
            Piece::Color winner = c.capturer.color();
        
            gameOver_ = true;
           std::cout << "🎯 GAME OVER! Winner: "
                  << (winner == Piece::Color::WHITE ? "WHITE" : "BLACK")
                  << std::endl;

        bus_.publish(
            GameOverEvent{winner, "king captured"}
        );
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

void GameEngine::updateBoardFromServer(const json& boardStateJson) {
  std::cout << "Updating board from server..." << std::endl;
   for (int row = 0; row < board_.rows(); ++row) {
        for (int col = 0; col < board_.cols(); ++col) {
            board_.setCell(row, col, std::nullopt);
        }
    }
    
    // Rebuild board from server state
    for (const auto& cellJson : boardStateJson.value("pieces", json::array())) {
        int row = cellJson.value("row", -1);
        int col = cellJson.value("col", -1);
        std::string colorStr = cellJson.value("color", "");
        int typeInt = cellJson.value("type", -1);
        std::cout
    << row << ","
    << col << " "
    << colorStr
    << " "
    << typeInt
    << std::endl;
        if (row >= 0 && col >= 0 && typeInt >= 0) {
            Piece::Color color = (colorStr == "WHITE") ? Piece::Color::WHITE : Piece::Color::BLACK;
            Piece::Type type = static_cast<Piece::Type>(typeInt);
            
            board_.setCell(row, col, Piece(color, type));
        }
    }
}

void GameEngine::startMotion(const Motion& motion)
{
    arbiter_.startMotion(motion);

    bus_.publish(MotionStartedEvent{
        motion.from,
        motion.to,
        motion.piece,
        motion.durationMs
    });
}

