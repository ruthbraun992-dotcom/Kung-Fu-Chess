#include <catch2/catch_test_macros.hpp>
#include "input/Controller.hpp"
#include "model/Board.hpp"
#include "model/Piece.hpp"
#include "engine/GameEngine.hpp"
#include "input/BoardMapper.hpp"

TEST_CASE("First click on a piece then second click requests a move", "[controller]") {
    Board board(5,5);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    GameEngine engine(std::move(board));
    Controller controller(engine);

    controller.click(50, 50);   // selects piece at (0,0)
    controller.click(250, 50);  // requests move to (0,2)

    engine.wait(2000);
    CHECK(engine.board().getCell(0,2).has_value() == true);
}

TEST_CASE("Click outside the board with no selection is ignored", "[controller]") {
    Board board(3,3);
    GameEngine engine(std::move(board));
    Controller controller(engine);

    controller.click(-10, -10); // should not crash, no-op
    SUCCEED();
}

TEST_CASE("Click outside the board while a piece is selected cancels selection", "[controller]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    GameEngine engine(std::move(board));
    Controller controller(engine);

    controller.click(50, 50);      // select
    controller.click(-100, -100);  // cancel selection, no move requested

    engine.wait(2000);
    CHECK(engine.board().getCell(0,0).has_value() == true); // unchanged
}