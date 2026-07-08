#include "Piece.hpp"
#include <iostream>

void check(const std::string& name, bool result, bool expected)
{
    if (result == expected)
        std::cout << name << " passed\n";
    else
        std::cout << name << " FAILED\n";
}


void testKing()
{
    Piece king(Piece::Color::WHITE, Piece::Type::KING);

    check(
        "King one step diagonal",
        king.isValidMove(4,4,5,5),
        true
    );

    check(
        "King two steps",
        king.isValidMove(4,4,6,6),
        false
    );

    check(
        "King same place",
        king.isValidMove(4,4,4,4),
        false
    );
}


void testRook()
{
    Piece rook(Piece::Color::WHITE, Piece::Type::ROOK);

    check(
        "Rook horizontal",
        rook.isValidMove(2,2,2,7),
        true
    );

    check(
        "Rook vertical",
        rook.isValidMove(2,2,7,2),
        true
    );

    check(
        "Rook diagonal",
        rook.isValidMove(2,2,5,5),
        false
    );
}


void testBishop()
{
    Piece bishop(Piece::Color::WHITE, Piece::Type::BISHOP);

    check(
        "Bishop diagonal",
        bishop.isValidMove(1,1,4,4),
        true
    );

    check(
        "Bishop straight",
        bishop.isValidMove(1,1,1,5),
        false
    );
}


void testQueen()
{
    Piece queen(Piece::Color::WHITE, Piece::Type::QUEEN);

    check(
        "Queen diagonal",
        queen.isValidMove(0,0,3,3),
        true
    );

    check(
        "Queen straight",
        queen.isValidMove(0,0,0,5),
        true
    );

    check(
        "Queen knight move",
        queen.isValidMove(0,0,2,1),
        false
    );
}


void testKnight()
{
    Piece knight(Piece::Color::WHITE, Piece::Type::KNIGHT);

    check(
        "Knight L move",
        knight.isValidMove(0,0,2,1),
        true
    );

    check(
        "Knight straight",
        knight.isValidMove(0,0,0,2),
        false
    );
}


int main()
{
    testKing();
    testRook();
    testBishop();
    testQueen();
    testKnight();

    return 0;
}