#include <catch2/catch_test_macros.hpp>
#include "model/Board.hpp"
#include "model/Piece.hpp"
#include "engine/GameEngine.hpp"

TEST_CASE("Valid move request is accepted", "[gameengine]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    GameEngine engine(std::move(board));
    CHECK(engine.requestMove(0,0,0,3) == true);
}

TEST_CASE("Illegal move request is rejected and board stays unchanged", "[gameengine]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    GameEngine engine(std::move(board));
    CHECK(engine.requestMove(0,0,3,3) == false); // diagonal, illegal for rook
    CHECK(engine.board().getCell(0,0).has_value() == true);
}

TEST_CASE("Second move is rejected while a motion is already active", "[gameengine]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(4,4, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));

    GameEngine engine(std::move(board));
    REQUIRE(engine.requestMove(0,0,0,3) == true);

    CHECK(engine.requestMove(4,4,2,2) == false); // motion_in_progress
}

TEST_CASE("Move completes on the board only after enough wait time", "[gameengine]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    GameEngine engine(std::move(board));
    engine.requestMove(0,0,0,2);

    engine.wait(2000);
    CHECK(engine.board().getCell(0,0).has_value() == false);
    CHECK(engine.board().getCell(0,2).has_value() == true);
}