#pragma once
#include "position.hpp"
#include "Piece.hpp"


struct Motion {
    Position from;
    Position to;
    Piece piece;
    long arrivalTime;
};