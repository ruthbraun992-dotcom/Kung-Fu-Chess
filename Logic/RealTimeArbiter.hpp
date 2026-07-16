#pragma once
#include "Motion.hpp"
#include "Board.hpp"
#include "position.hpp"
#include "Piece.hpp"
#include "RenderPosition.hpp"

#include <vector>

class RealTimeArbiter {
public:
    void startMotion(Motion m);
    void startJump(Position at, Piece piece, long durationMs);

    // מחזיר את כל הכלים שנתפסו (בין אם ע"י דריסה רגילה או ע"י תפיסה באוויר)
    std::vector<Piece> advanceTime(long ms, Board& board);

    bool hasActiveMotionFrom(const Position& from) const;
    bool conflictsWithActiveMotion(const Position& from, const Position& to) const;
    bool isJumpingAt(const Position& pos) const;
    std::optional<RenderPosition> currentPositionOf(const Position& from) const;

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

    long now_ = 0;
    std::vector<ActiveMotion> motions_;
    std::vector<ActiveJump> jumps_;
};