#include <catch2/catch_test_macros.hpp>
#include "GameEngine.hpp"
#include "Board.hpp"
#include "Piece.hpp"

TEST_CASE("Pawn: can move 2 cells forward from start row") {
    Board board(8, 8);
    board.setCell(6, 0, Piece::fromToken("wP").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(6, 0, 4, 0));
}

TEST_CASE("Pawn: cannot move 2 cells forward when not on start row") {
    Board board(8, 8);
    board.setCell(5, 0, Piece::fromToken("wP").value());
    GameEngine engine(std::move(board));

    REQUIRE_FALSE(engine.requestMove(5, 0, 3, 0));
}

TEST_CASE("Pawn: 2-cell move blocked by piece in path") {
    Board board(8, 8);
    board.setCell(6, 0, Piece::fromToken("wP").value());
    board.setCell(5, 0, Piece::fromToken("bR").value());
    GameEngine engine(std::move(board));

    REQUIRE_FALSE(engine.requestMove(6, 0, 4, 0));
}

TEST_CASE("Pawn: promotes to queen upon reaching last row") {
    Board board(8, 8);
    board.setCell(1, 0, Piece::fromToken("wP").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(1, 0, 0, 0));
    engine.wait(1000);

    auto promoted = engine.board().getCell(0, 0);
    REQUIRE(promoted.has_value());
    REQUIRE(promoted->type() == Piece::Type::QUEEN);
    REQUIRE(promoted->color() == Piece::Color::WHITE);
}