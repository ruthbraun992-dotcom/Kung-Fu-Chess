#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <optional>
#include <vector>
#include <utility>
#include "PieceState.hpp"

class Piece {
public:
    enum class Color { WHITE, BLACK };
    enum class Type  { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

    Piece(Color color, Type type);

    Color color() const { return color_; }
    Type type() const { return type_; }

    std::string toString() const;

    static std::optional<Piece> fromToken(const std::string& token);

    PieceState state() const { return state_;}

    void setState(PieceState state){ state_ = state; }

private:
    PieceState state_ = PieceState::IDLE;
    Color color_;
    Type type_;

    static char typeToChar(Type t);
    static std::optional<Color> charToColor(char c);
    static std::optional<Type> charToType(char c);
};

#endif