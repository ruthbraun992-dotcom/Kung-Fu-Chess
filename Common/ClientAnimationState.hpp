#pragma once
#include <map>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>
#include "Position.hpp"
#include "Piece.hpp"
#include "RenderPosition.hpp"
#include "PieceState.hpp"

struct PieceRenderState
{
    std::optional<RenderPosition> renderPos;
    std::optional<PieceState>     state;
    std::optional<long>           stateStartTime; // בשעון הלקוח
    std::optional<long>           stateDuration;
};

struct ActiveMotion
{
    Position from;
    Position to;
      PieceState state;
    long     startTime;   // בשעון הלקוח
    long     durationMs;
};
struct ActiveJump
{
    Position pos;
    Piece piece;

    long startTime;
    long durationMs;
};
class ClientAnimationState
{
public:
    void updateFromMessage(const nlohmann::json& boardStateMsg);
    void startMotion(const Position& from, const Position& to, PieceState state, long durationMs);

    std::optional<RenderPosition> currentPositionOf(const Position& pos) const;
    std::optional<PieceState>     currentStateOf(const Position& pos) const;
    std::optional<long>           stateStartTimeOf(const Position& pos) const;
    std::optional<long>           stateDurationOf(const Position& pos) const;
    long currentTime() const;
    void startJump(const Position& pos, const Piece& piece,long duration);
    

private:
    std::map<Position, PieceRenderState> pieces_;
    mutable std::map<Position, ActiveMotion> activeMotions_;
    mutable std::map<Position, ActiveJump> activeJumps_;
};