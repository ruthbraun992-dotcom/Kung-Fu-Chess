#pragma once
#include "Piece.hpp"
#include "Position.hpp"

struct CaptureEvent
{
    Piece capturedPiece;
    Piece capturer;
    Position at;
};