#include <catch2/catch_test_macros.hpp>
#include "Piece.hpp"

TEST_CASE("King moves", "[piece][king]") {
    Piece king(Piece::Color::WHITE, Piece::Type::KING);

    CHECK(king.isValidShape(4,4,5,5) == true);   // one step diagonal
    CHECK(king.isValidShape(4,4,6,6) == false);  // two steps
    CHECK(king.isValidShape(4,4,4,4) == false);  // same place
}

TEST_CASE("Rook moves", "[piece][rook]") {
    Piece rook(Piece::Color::WHITE, Piece::Type::ROOK);

    CHECK(rook.isValidShape(2,2,2,7) == true);   // horizontal
    CHECK(rook.isValidShape(2,2,7,2) == true);   // vertical
    CHECK(rook.isValidShape(2,2,5,5) == false);  // diagonal
}

TEST_CASE("Bishop moves", "[piece][bishop]") {
    Piece bishop(Piece::Color::WHITE, Piece::Type::BISHOP);

    CHECK(bishop.isValidShape(1,1,4,4) == true);   // diagonal
    CHECK(bishop.isValidShape(1,1,1,5) == false);  // straight
}

TEST_CASE("Queen moves", "[piece][queen]") {
    Piece queen(Piece::Color::WHITE, Piece::Type::QUEEN);

    CHECK(queen.isValidShape(0,0,3,3) == true);   // diagonal
    CHECK(queen.isValidShape(0,0,0,5) == true);   // straight
    CHECK(queen.isValidShape(0,0,2,1) == false);  // knight-shaped move
}

TEST_CASE("Knight moves", "[piece][knight]") {
    Piece knight(Piece::Color::WHITE, Piece::Type::KNIGHT);

    CHECK(knight.isValidShape(0,0,2,1) == true);   // L move
    CHECK(knight.isValidShape(0,0,0,2) == false);  // straight
}