#include "GameStats.hpp"
#include "CaptureEvent.hpp"

void GameStats::recordMove(Piece::Color color,
                           Piece::Type type,
                           Position from,
                           Position to,
                           bool isJump)
{
    moves_.push_back({color, type, from, to, isJump});
}

void GameStats::recordCapture(const CaptureEvent& event)
{
    scores_[event.capturer.color()] +=
        valueOf(event.capturedPiece.type());
}

int GameStats::score(Piece::Color color) const
{
    auto it = scores_.find(color);
    return (it != scores_.end()) ? it->second : 0;
}

const std::vector<MoveRecord>& GameStats::moves() const
{
    return moves_;
}

int GameStats::valueOf(Piece::Type type)
{
    switch (type)
    {
        case Piece::Type::PAWN:   return 1;
        case Piece::Type::KNIGHT: return 3;
        case Piece::Type::BISHOP: return 3;
        case Piece::Type::ROOK:   return 5;
        case Piece::Type::QUEEN:  return 9;
        case Piece::Type::KING:   return 0;
    }
    return 0;
}