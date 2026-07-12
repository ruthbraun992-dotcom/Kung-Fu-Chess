#include "RealTimeArbiter.hpp"

void RealTimeArbiter::startMotion(Motion m) {
    long duration = m.arrivalTime; // השדה הזה מכיל את משך התנועה (ms), לא זמן מוחלט
    motions_.push_back(ActiveMotion{m, now_ + duration});
}

void RealTimeArbiter::advanceTime(long ms, Board& board) {
    now_ += ms;

    for (auto it = motions_.begin(); it != motions_.end(); ) {
        if (now_ >= it->arrivalAt) {
            board.setCell(it->motion.from.row, it->motion.from.col, std::nullopt);
            board.setCell(it->motion.to.row, it->motion.to.col, it->motion.piece);
            it = motions_.erase(it);
        } else {
            ++it;
        }
    }
}

bool RealTimeArbiter::hasActiveMotionFrom(const Position& from) const {
    for (const auto& am : motions_) {
        if (am.motion.from.row == from.row && am.motion.from.col == from.col) {
            return true;
        }
    }
    return false;
}

bool RealTimeArbiter::conflictsWithActiveMotion(const Position& from, const Position& to) const {
    for (const auto& am : motions_) {
        // אותו כלי כבר בתנועה
        if (am.motion.from.row == from.row && am.motion.from.col == from.col) return true;
        // מישהו אחר כבר בדרך לאותו יעד
        if (am.motion.to.row == to.row && am.motion.to.col == to.col) return true;
        // היעד שלי הוא המקור של תזוזה פעילה אחרת (המשבצת עדיין "תפוסה" עד ההגעה)
        if (am.motion.from.row == to.row && am.motion.from.col == to.col) return true;
    }
    return false;
}