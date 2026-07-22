#pragma once
#include "position.hpp"
#include "Piece.hpp"
#include "PieceState.hpp"


struct Motion
{
    Position from;
    Position to;
    Piece piece;

    PieceState state;

    long startTime;
    long durationMs;
};