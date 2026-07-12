#pragma once
#include "Motion.hpp"
#include "Board.hpp"
#include "position.hpp"

#include <vector>

class RealTimeArbiter {
public:
    void startMotion(Motion m);
    void advanceTime(long ms, Board& board);

    // חוסם premove: הכלי במשבצת from כבר באמצע תזוזה
    bool hasActiveMotionFrom(const Position& from) const;

    // חוסם movement conflict: יעד/מקור מתנגשים עם תזוזה פעילה אחרת
    bool conflictsWithActiveMotion(const Position& from, const Position& to) const;

private:
    struct ActiveMotion {
        Motion motion;
        long arrivalAt;
    };

    long now_ = 0;
    std::vector<ActiveMotion> motions_;
};