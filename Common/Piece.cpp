#include "Piece.hpp"
#include <vector>
#include <utility>

Piece::Piece(Color color, Type type) : color_(color), type_(type) {}

std::string Piece::toString() const {
    std::string s;
    s += (color_ == Color::WHITE ? 'w' : 'b');
    s += typeToChar(type_);
    return s;
}

std::optional<Piece> Piece::fromToken(const std::string& token) {
    if (token.size() != 2) return std::nullopt;

    char colorChar = token[0];
    char typeChar  = token[1];

    std::optional<Color> color = charToColor(colorChar);
    std::optional<Type>  type  = charToType(typeChar);

    if (!color.has_value() || !type.has_value()) return std::nullopt;
    return Piece(*color, *type);
}

char Piece::typeToChar(Type t) {
    switch (t) {
        case Type::KING:   return 'K';
        case Type::QUEEN:  return 'Q';
        case Type::ROOK:   return 'R';
        case Type::BISHOP: return 'B';
        case Type::KNIGHT: return 'N';
        case Type::PAWN:   return 'P';
    }
    return '?'; 
}

std::optional<Piece::Color> Piece::charToColor(char c) {
    if (c == 'w') return Color::WHITE;
    if (c == 'b') return Color::BLACK;
    return std::nullopt;
}

std::optional<Piece::Type> Piece::charToType(char c) {
    switch (c) {
        case 'K': return Type::KING;
        case 'Q': return Type::QUEEN;
        case 'R': return Type::ROOK;
        case 'B': return Type::BISHOP;
        case 'N': return Type::KNIGHT;
        case 'P': return Type::PAWN;
        default:  return std::nullopt;
    }
}
