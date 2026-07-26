#pragma once
#include <string>
#include "Piece.hpp"
#include "Position.hpp"

struct ScoreUpdatedEvent {
    Piece::Color color;
    int newScore;
};

struct MoveLoggedEvent {
    Piece::Color color;
    std::string moveText;
};

struct PieceCapturedEvent {
    Piece::Color capturedColor;
    Piece::Type pieceType;
    Position atSquare;
};

enum class SoundType { Move, Capture, GameStart, GameEnd };
struct SoundEvent {
    SoundType type;
};

struct GameStartedEvent {};

struct GameEndedEvent {
    Piece::Color winner;
};

struct GameOverEvent {
    Piece::Color winner;
    std::string  reason;
};

