#include <catch2/catch_test_macros.hpp>
#include "Board.hpp"

TEST_CASE("King cannot move two steps", "[board][king]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::KING));

    CHECK(board.isValidMove(0,0,2,2) == false);
}

TEST_CASE("Rook cannot move diagonally", "[board][rook]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    CHECK(board.isValidMove(0,0,1,1) == false);
}

TEST_CASE("Rook is blocked by a piece in its path", "[board][rook]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(0,2, Piece(Piece::Color::WHITE, Piece::Type::BISHOP)); // חוסם באמצע

    CHECK(board.isValidMove(0,0,0,4) == false);
}

TEST_CASE("Bishop is blocked by a piece in its path", "[board][bishop]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));
    board.setCell(1,1, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    CHECK(board.isValidMove(0,0,3,3) == false);
}

TEST_CASE("Knight jumps over pieces", "[board][knight]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::KNIGHT));
    board.setCell(1,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK)); // לא אמור להפריע

    CHECK(board.isValidMove(0,0,2,1) == true);
}

TEST_CASE("Cannot capture a friendly piece", "[board][capture]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(0,2, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));

    CHECK(board.isValidMove(0,0,0,2) == false);
}

TEST_CASE("Can capture an enemy piece", "[board][capture]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(0,2, Piece(Piece::Color::BLACK, Piece::Type::BISHOP));

    CHECK(board.isValidMove(0,0,0,2) == true);
}