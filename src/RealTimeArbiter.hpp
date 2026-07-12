#pragma once
#include "Motion.hpp"
#include "Board.hpp"
#include "position.hpp"
#include "Piece.hpp"

#include <vector>

class RealTimeArbiter {
public:
    void startMotion(Motion m);

    // מחזיר את כל הכלים שנתפסו (נדרסו) בקריאה הזו
    std::vector<Piece> advanceTime(long ms, Board& board);

    bool hasActiveMotionFrom(const Position& from) const;
    bool conflictsWithActiveMotion(const Position& from, const Position& to) const;

private:
    struct ActiveMotion {
        Motion motion;
        long arrivalAt;
    };

    long now_ = 0;
    std::vector<ActiveMotion> motions_;
};