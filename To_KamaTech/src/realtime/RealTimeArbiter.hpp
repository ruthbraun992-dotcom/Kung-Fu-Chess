// RealTimeArbiter.hpp
#pragma once
#include "Motion.hpp"
#include "model/Board.hpp"

#include <optional>

class RealTimeArbiter {
public:
    bool hasActiveMotion() const { return motion_.has_value(); }
    void startMotion(Motion m) { motion_ = m; }
    void advanceTime(long ms, Board& board);

private:
    long now_ = 0;
    std::optional<Motion> motion_;
};