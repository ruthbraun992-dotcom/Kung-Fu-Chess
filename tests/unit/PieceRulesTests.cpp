#include <catch2/catch_test_macros.hpp>
#include "Piece.hpp"
#include "PieceRules.hpp"

TEST_CASE("King moves one step in any direction", "[rules][king]") {
    Piece king(Piece::Color::WHITE, Piece::Type::KING);

    CHECK(PieceRules::isValidShape(king, 4,4,5,5) == true);
    CHECK(PieceRules::isValidShape(king, 4,4,6,6) == false);
    CHECK(PieceRules::isValidShape(king, 4,4,4,4) == false);
}

TEST_CASE("Rook moves horizontally or vertically only", "[rules][rook]") {
    Piece rook(Piece::Color::WHITE, Piece::Type::ROOK);

    CHECK(PieceRules::isValidShape(rook, 2,2,2,7) == true);
    CHECK(PieceRules::isValidShape(rook, 2,2,7,2) == true);
    CHECK(PieceRules::isValidShape(rook, 2,2,5,5) == false);
}

TEST_CASE("Bishop moves diagonally only", "[rules][bishop]") {
    Piece bishop(Piece::Color::WHITE, Piece::Type::BISHOP);

    CHECK(PieceRules::isValidShape(bishop, 1,1,4,4) == true);
    CHECK(PieceRules::isValidShape(bishop, 1,1,1,5) == false);
}

TEST_CASE("Queen combines rook and bishop movement", "[rules][queen]") {
    Piece queen(Piece::Color::WHITE, Piece::Type::QUEEN);

    CHECK(PieceRules::isValidShape(queen, 0,0,3,3) == true);
    CHECK(PieceRules::isValidShape(queen, 0,0,0,5) == true);
    CHECK(PieceRules::isValidShape(queen, 0,0,2,1) == false);
}

TEST_CASE("Knight moves in an L shape", "[rules][knight]") {
    Piece knight(Piece::Color::WHITE, Piece::Type::KNIGHT);

    CHECK(PieceRules::isValidShape(knight, 0,0,2,1) == true);
    CHECK(PieceRules::isValidShape(knight, 0,0,0,2) == false);
}

TEST_CASE("Pawn path is empty for adjacent shapes", "[rules][pawn][path]") {
    Piece pawn(Piece::Color::WHITE, Piece::Type::PAWN);
    auto path = PieceRules::getPath(pawn, 6,4,5,4);
    CHECK(path.empty());
}

TEST_CASE("Rook path includes every intermediate cell", "[rules][rook][path]") {
    Piece rook(Piece::Color::WHITE, Piece::Type::ROOK);
    auto path = PieceRules::getPath(rook, 0,0,0,3);

    REQUIRE(path.size() == 2);
    CHECK(path[0] == std::make_pair(0,1));
    CHECK(path[1] == std::make_pair(0,2));
}