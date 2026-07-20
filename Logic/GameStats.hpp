#pragma once

#include "Piece.hpp"
#include "Position.hpp"
#include "CaptureEvent.hpp"

#include <vector>
#include <map>

struct MoveRecord {
    Piece::Color color;
    Piece::Type pieceType;
    Position from;
    Position to;
    bool isJump;
};

class GameStats {
public:
    void recordMove(Piece::Color color,
                    Piece::Type type,
                    Position from,
                    Position to,
                    bool isJump);

    void recordCapture(const CaptureEvent& event);

    int score(Piece::Color color) const;
    const std::vector<MoveRecord>& moves() const;

private:
    static int valueOf(Piece::Type type);

    std::vector<MoveRecord> moves_;
    std::map<Piece::Color, int> scores_;
};