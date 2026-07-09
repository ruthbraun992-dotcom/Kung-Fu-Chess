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
    return '?'; // unreachable
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
#include <cmath>

bool Piece::isValidShape(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) const
{
    int rowDiff = std::abs(toRow - fromRow);
    int colDiff = std::abs(toCol - fromCol);

    switch (type_)
    {
        case Type::KING:
            return rowDiff <= 1 && colDiff <= 1 
                   && (rowDiff != 0 || colDiff != 0);

        case Type::ROOK:
            return fromRow == toRow || fromCol == toCol;

        case Type::BISHOP:
            return rowDiff == colDiff;

        case Type::QUEEN:
            return (fromRow == toRow || fromCol == toCol)
                   || rowDiff == colDiff;

        case Type::KNIGHT:
            return (rowDiff == 2 && colDiff == 1) ||
                   (rowDiff == 1 && colDiff == 2);

        case Type::PAWN:
            {
                int rowStep = toRow - fromRow;
                int colStep = std::abs(toCol - fromCol);

                if (color_ == Color::WHITE)
                {
                    return rowStep == -1 && colStep <= 1;
                }
                else
                {
                    return rowStep == 1 && colStep <= 1;
                }
            }
    }

    return false;
}
std::vector<std::pair<int,int>> Piece::getPath(
    int fromRow,
    int fromCol,
    int toRow,
    int toCol
) const
{
    std::vector<std::pair<int,int>> path;

    int rowStep = 0;
    int colStep = 0;

    switch(type_)
    {
        case Type::ROOK:
        case Type::QUEEN:

            if (fromRow == toRow)
                colStep = (toCol > fromCol) ? 1 : -1;

            else if (fromCol == toCol)
                rowStep = (toRow > fromRow) ? 1 : -1;

            break;


        case Type::BISHOP:

            rowStep = (toRow > fromRow) ? 1 : -1;
            colStep = (toCol > fromCol) ? 1 : -1;

            break;


        // אין משבצות בדרך
        case Type::KING:
        case Type::KNIGHT:
        case Type::PAWN:
            return path;
    }


    int row = fromRow + rowStep;
    int col = fromCol + colStep;


    while(row != toRow || col != toCol)
    {
        path.push_back({row, col});

        row += rowStep;
        col += colStep;
    }

    return path;
}