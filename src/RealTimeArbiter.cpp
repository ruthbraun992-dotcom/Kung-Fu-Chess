// RealTimeArbiter.cpp
#include "RealTimeArbiter.hpp"

void RealTimeArbiter::advanceTime(long ms, Board& board) {
    now_ += ms;
    if (motion_.has_value() && now_ >= motion_->arrivalTime) {
                board.setCell(
            motion_->from.row,
            motion_->from.col,
            std::nullopt
        );

        board.setCell(
            motion_->to.row,
            motion_->to.col,
            motion_->piece
        );

        motion_.reset();
    }
}