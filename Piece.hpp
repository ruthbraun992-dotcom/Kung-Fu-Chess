#ifndef PIECE_H
#define PIECE_H

#include <string>
#include <optional>
#include <cmath>

// Represents a single chess piece: a color + a type.
// Does not know anything about the board, timing, or rules.
class Piece {
public:
    enum class Color { WHITE, BLACK };
    enum class Type  { KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN };

    Piece(Color color, Type type);

    Color color() const { return color_; }
    Type  type()  const { return type_; }

    // Canonical 2-character representation, e.g. "wK", "bQ"
    std::string toString() const;

    // Tries to build a Piece from a 2-character token like "wK".
    // Returns std::nullopt if the token is not a valid piece token.
    static std::optional<Piece> fromToken(const std::string& token);
    bool isValidMove(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
    ) const;

private:
    Color color_;
    Type  type_;

    static char typeToChar(Type t);
    static std::optional<Color> charToColor(char c);
    static std::optional<Type>  charToType(char c);
};

#endif // PIECE_H