#pragma once
#include "Motion.hpp"
#include "Board.hpp"
#include "position.hpp"
#include "Piece.hpp"
#include "RenderPosition.hpp"
#include "AnimationConfigLoader.hpp"
#include "PieceState.hpp"
#include "CaptureEvent.hpp"

#include <vector>
#include <optional>

class RealTimeArbiter {
public:
    explicit RealTimeArbiter(const AnimationConfigLoader& configs) : configs_(configs) {}

    void startMotion(Motion m);
    void startJump(Position at, Piece piece, long durationMs);

    std::vector<CaptureEvent> advanceTime(long ms, Board& board);

    bool hasActiveMotionFrom(const Position& from) const;
    bool conflictsWithActiveMotion(const Position& from, const Position& to) const;
    bool isJumpingAt(const Position& pos) const;
    std::optional<RenderPosition> currentPositionOf(const Position& from) const;
    long currentTime() const;
    std::optional<PieceState> currentStateOf(const Position& from) const;
    std::optional<long> stateStartTimeOf(const Position& from) const;   // חדש

private:
    struct ActiveMotion {
        Motion motion;
        long arrivalAt;
    };
    struct ActiveJump {
        Position at;
        Piece piece;
        long arrivalAt;
    };

    void startFollowUpState(const Position& at, Piece piece, PieceState state);

    const AnimationConfigLoader& configs_;
    long now_ = 0;
    std::vector<ActiveMotion> motions_;
    std::vector<ActiveJump> jumps_;
};