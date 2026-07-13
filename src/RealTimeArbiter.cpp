#include "RealTimeArbiter.hpp"

void RealTimeArbiter::startMotion(Motion m) {
    long duration = m.arrivalTime;
    motions_.push_back(ActiveMotion{m, now_ + duration});
}

void RealTimeArbiter::startJump(Position at, Piece piece, long durationMs) {
    jumps_.push_back(ActiveJump{at, piece, now_ + durationMs});
}

std::vector<Piece> RealTimeArbiter::advanceTime(long ms, Board& board) {
    now_ += ms;
    std::vector<Piece> captured;

    // שלב 1: פתרון תנועות שהגיעו ליעד - כולל בדיקת תפיסה-באוויר
    for (auto it = motions_.begin(); it != motions_.end(); ) {
        if (now_ >= it->arrivalAt) {
            Position to = it->motion.to;
            bool caughtByJumper = false;

            for (const auto& jump : jumps_) {
                if (jump.at.row == to.row && jump.at.col == to.col &&
                    jump.piece.color() != it->motion.piece.color()) {
                    captured.push_back(it->motion.piece); // הכלי המגיע נתפס
                    caughtByJumper = true;
                    break;
                }
            }

            board.setCell(it->motion.from.row, it->motion.from.col, std::nullopt);

            if (!caughtByJumper) {
                auto existing = board.getCell(to.row, to.col);
                if (existing.has_value()) captured.push_back(*existing);

                Piece landedPiece = it->motion.piece;
                int lastRow = (landedPiece.color() == Piece::Color::WHITE) ? 0 : board.rows() - 1;
                if (landedPiece.type() == Piece::Type::PAWN && to.row == lastRow) {
                    landedPiece = Piece(landedPiece.color(), Piece::Type::QUEEN);
                }
                board.setCell(to.row, to.col, landedPiece);
            }
            // אם caughtByJumper==true - אין שינוי בתא היעד, הכלי הקופץ נשאר כפי שהוא

            it = motions_.erase(it);
        } else {
            ++it;
        }
    }

    // שלב 2: קפיצות שהסתיימו בלי שנתפס בהן אויב - נחיתה שקטה, בלי שינוי בלוח
    for (auto it = jumps_.begin(); it != jumps_.end(); ) {
        if (now_ >= it->arrivalAt) {
            it = jumps_.erase(it);
        } else {
            ++it;
        }
    }

    return captured;
}

bool RealTimeArbiter::hasActiveMotionFrom(const Position& from) const {
    for (const auto& am : motions_) {
        if (am.motion.from.row == from.row && am.motion.from.col == from.col) return true;
    }
    return false;
}

bool RealTimeArbiter::conflictsWithActiveMotion(const Position& from, const Position& to) const {
    for (const auto& am : motions_) {
        if (am.motion.from.row == from.row && am.motion.from.col == from.col) return true;
        if (am.motion.to.row == to.row && am.motion.to.col == to.col) return true;
        if (am.motion.from.row == to.row && am.motion.from.col == to.col) return true;
    }
    return false;
}

bool RealTimeArbiter::isJumpingAt(const Position& pos) const {
    for (const auto& j : jumps_) {
        if (j.at.row == pos.row && j.at.col == pos.col) return true;
    }
    return false;
}