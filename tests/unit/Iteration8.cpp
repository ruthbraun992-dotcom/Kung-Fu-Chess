#include <catch2/catch_test_macros.hpp>
#include "GameEngine.hpp"
#include "Board.hpp"
#include "Piece.hpp"

TEST_CASE("GameEngine: capturing the enemy king ends the game") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    board.setCell(0, 2, Piece::fromToken("bK").value());
    GameEngine engine(std::move(board));

    REQUIRE_FALSE(engine.isGameOver());
    REQUIRE(engine.requestMove(0, 0, 0, 2)); // wR נע לכיוון bK
    engine.wait(2000);                       // ההגעה מתבצעת -> תפיסה

    REQUIRE(engine.isGameOver());
}

TEST_CASE("GameEngine: moves after game over are ignored") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    board.setCell(0, 2, Piece::fromToken("bK").value());
    board.setCell(2, 0, Piece::fromToken("bR").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2));
    engine.wait(2000); // המלך נתפס, המשחק נגמר

    REQUIRE(engine.isGameOver());
    REQUIRE_FALSE(engine.requestMove(2, 0, 2, 2)); // מהלך אחרי סיום - נדחה
}

TEST_CASE("GameEngine: non-king capture does not end the game") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    board.setCell(0, 2, Piece::fromToken("bR").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2));
    engine.wait(2000);

    REQUIRE_FALSE(engine.isGameOver());
}