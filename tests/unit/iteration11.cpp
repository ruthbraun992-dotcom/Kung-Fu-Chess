#include <catch2/catch_test_macros.hpp>
#include "GameEngine.hpp"
#include "Board.hpp"
#include "Piece.hpp"

TEST_CASE("Jump: airborne piece captures arriving enemy") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    board.setCell(0, 2, Piece::fromToken("bR").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestJump(0, 0));         // wR קופץ במקום
    REQUIRE(engine.requestMove(0, 2, 0, 0));    // bR זז לעברו
    engine.wait(2000);                          // ה-bR "מגיע" תוך כדי שwR עדיין באוויר

    auto cell = engine.board().getCell(0, 0);
    REQUIRE(cell.has_value());
    REQUIRE(cell->color() == Piece::Color::WHITE); // wR נשאר, bR נעלם
}

TEST_CASE("Jump: piece lands normally if no enemy arrives") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestJump(0, 0));
    engine.wait(1000);

    auto cell = engine.board().getCell(0, 0);
    REQUIRE(cell.has_value());
    REQUIRE_FALSE(engine.isGameOver());
}

TEST_CASE("Jump: a moving piece cannot jump") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2));
    REQUIRE_FALSE(engine.requestJump(0, 0)); // הכלי עדיין בתנועה
}

TEST_CASE("Jump: cannot jump from an empty cell") {
    Board board(3, 3);
    GameEngine engine(std::move(board));

    REQUIRE_FALSE(engine.requestJump(1, 1));
}

TEST_CASE("Jump: capturing enemy king mid-jump ends the game") {
    Board board(3, 3);
    board.setCell(0, 0, Piece::fromToken("wR").value());
    board.setCell(0, 2, Piece::fromToken("bK").value());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestJump(0, 0));
    REQUIRE(engine.requestMove(0, 2, 0, 0));
    engine.wait(2000);

    REQUIRE(engine.isGameOver());
}