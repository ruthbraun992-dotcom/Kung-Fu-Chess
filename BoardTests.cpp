#include "Board.hpp"
#include <iostream>

void check(std::string name, bool result, bool expected)
{
    if (result == expected)
        std::cout << name << " PASSED\n";
    else
        std::cout << name << " FAILED\n";
}


void testKingTwoSteps()
{
    Board board(3,3);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::KING)
    );

    check(
        "King two steps invalid",
        board.isValidMove(0,0,2,2),
        false
    );
}


void testRookDiagonal()
{
    Board board(3,3);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::ROOK)
    );

    check(
        "Rook diagonal invalid",
        board.isValidMove(0,0,1,1),
        false
    );
}


void testRookBlocked()
{
    Board board(5,5);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::ROOK)
    );

    // כלי שחוסם באמצע
    board.setCell(
        0,2,
        Piece(Piece::Color::WHITE, Piece::Type::BISHOP)
    );

    check(
        "Rook blocked",
        board.isValidMove(0,0,0,4),
        false
    );
}


void testBishopBlocked()
{
    Board board(5,5);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::BISHOP)
    );

    board.setCell(
        1,1,
        Piece(Piece::Color::WHITE, Piece::Type::ROOK)
    );

    check(
        "Bishop blocked",
        board.isValidMove(0,0,3,3),
        false
    );
}


void testKnightJump()
{
    Board board(5,5);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::KNIGHT)
    );

    // כלי באמצע - לא אמור להפריע לפרש
    board.setCell(
        1,0,
        Piece(Piece::Color::WHITE, Piece::Type::ROOK)
    );


    check(
        "Knight jumps over pieces",
        board.isValidMove(0,0,2,1),
        true
    );
}


void testCannotCaptureSameColor()
{
    Board board(3,3);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::ROOK)
    );

    board.setCell(
        0,2,
        Piece(Piece::Color::WHITE, Piece::Type::BISHOP)
    );


    check(
        "Cannot capture friendly piece",
        board.isValidMove(0,0,0,2),
        false
    );
}


void testCanCaptureEnemy()
{
    Board board(3,3);

    board.setCell(
        0,0,
        Piece(Piece::Color::WHITE, Piece::Type::ROOK)
    );

    board.setCell(
        0,2,
        Piece(Piece::Color::BLACK, Piece::Type::BISHOP)
    );


    check(
        "Can capture enemy",
        board.isValidMove(0,0,0,2),
        true
    );
}


int main()
{
    testKingTwoSteps();
    testRookDiagonal();
    testRookBlocked();
    testBishopBlocked();
    testKnightJump();
    testCannotCaptureSameColor();
    testCanCaptureEnemy();

    return 0;
}