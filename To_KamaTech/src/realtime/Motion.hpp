#pragma once
#include "model/Position.hpp"
#include "model/Piece.hpp"


struct Motion {
    Position from;
    Position to;
    Piece piece;
    long arrivalTime;
};