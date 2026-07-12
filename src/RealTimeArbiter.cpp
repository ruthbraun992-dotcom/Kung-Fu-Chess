#include "RealTimeArbiter.hpp"
#include "Piece.hpp"
void RealTimeArbiter::startMotion(Motion m) {
    long duration = m.arrivalTime;
    motions_.push_back(ActiveMotion{m, now_ + duration});
}

std::vector<Piece> RealTimeArbiter::advanceTime(long ms, Board& board) {
    now_ += ms;
    std::vector<Piece> captured;

    for (auto it = motions_.begin(); it != motions_.end(); ) {
        if (now_ >= it->arrivalAt) {
            auto existing = board.getCell(it->motion.to.row, it->motion.to.col);
            if (existing.has_value()) {
                captured.push_back(*existing); // הכלי שהיה ביעד נתפס עכשיו
            }
            board.setCell(it->motion.from.row, it->motion.from.col, std::nullopt);

            Piece landedPiece = it->motion.piece;
            int lastRow = (landedPiece.color() == Piece::Color::WHITE) ? 0 : board.rows() - 1;

            if (landedPiece.type() == Piece::Type::PAWN && it->motion.to.row == lastRow) {
                landedPiece = Piece(landedPiece.color(),Piece::Type::QUEEN); // ⚠ תלוי בבנאי Piece שלך
            }

            board.setCell(it->motion.to.row, it->motion.to.col, landedPiece);
            it = motions_.erase(it);
            
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