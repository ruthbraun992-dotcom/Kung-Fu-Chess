#pragma once

#include <string>

enum class PieceState
{
    IDLE,
    MOVE,
    JUMP,
    SHORT_REST,
    LONG_REST
};

inline std::string stateToString(PieceState state)
{
    switch(state)
    {
        case PieceState::IDLE:       return "idle";
        case PieceState::MOVE:       return "move";
        case PieceState::JUMP:       return "jump";
        case PieceState::SHORT_REST: return "short_rest";
        case PieceState::LONG_REST:  return "long_rest";
    }
    return "idle";
}

inline PieceState stringToState(const std::string& s)
{
    if (s == "idle")       return PieceState::IDLE;
    if (s == "move")       return PieceState::MOVE;
    if (s == "jump")       return PieceState::JUMP;
    if (s == "short_rest") return PieceState::SHORT_REST;
    if (s == "long_rest")  return PieceState::LONG_REST;
    return PieceState::IDLE;
}