#include <catch2/catch_test_macros.hpp>
#include "model/Board.hpp"
#include "model/Piece.hpp"
#include "rules/RuleEngine.hpp"

TEST_CASE("King cannot move two steps", "[ruleengine][king]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::KING));

    CHECK(RuleEngine::validateMove(board, 0,0,2,2) == false);
}

TEST_CASE("Rook cannot move diagonally", "[ruleengine][rook]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    CHECK(RuleEngine::validateMove(board, 0,0,1,1) == false);
}

TEST_CASE("Rook is blocked by a piece in its path", "[ruleengine][rook]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(0,2, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));

    CHECK(RuleEngine::validateMove(board, 0,0,0,4) == false);
}

TEST_CASE("Bishop is blocked by a piece in its path", "[ruleengine][bishop]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));
    board.setCell(1,1, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    CHECK(RuleEngine::validateMove(board, 0,0,3,3) == false);
}

TEST_CASE("Knight jumps over pieces", "[ruleengine][knight]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::KNIGHT));
    board.setCell(1,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    CHECK(RuleEngine::validateMove(board, 0,0,2,1) == true);
}

TEST_CASE("Cannot capture a friendly piece", "[ruleengine][capture]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(0,2, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));

    CHECK(RuleEngine::validateMove(board, 0,0,0,2) == false);
}

TEST_CASE("Can capture an enemy piece", "[ruleengine][capture]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(0,2, Piece(Piece::Color::BLACK, Piece::Type::BISHOP));

    CHECK(RuleEngine::validateMove(board, 0,0,0,2) == true);
}

TEST_CASE("Pawn moves forward only onto an empty cell", "[ruleengine][pawn]") {
    Board board(5,5);
    board.setCell(3,2, Piece(Piece::Color::WHITE, Piece::Type::PAWN));
    board.setCell(2,2, Piece(Piece::Color::BLACK, Piece::Type::ROOK)); // blocks forward

    CHECK(RuleEngine::validateMove(board, 3,2,2,2) == false);
}

TEST_CASE("Pawn captures diagonally only onto an occupied enemy cell", "[ruleengine][pawn]") {
    Board board(5,5);
    board.setCell(3,2, Piece(Piece::Color::WHITE, Piece::Type::PAWN));
    board.setCell(2,3, Piece(Piece::Color::BLACK, Piece::Type::ROOK));

    CHECK(RuleEngine::validateMove(board, 3,2,2,3) == true);  // capture
    CHECK(RuleEngine::validateMove(board, 3,2,2,1) == false); // diagonal, empty target
}