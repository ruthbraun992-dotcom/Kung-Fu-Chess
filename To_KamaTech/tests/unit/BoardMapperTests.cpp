#include <catch2/catch_test_macros.hpp>
#include "model/Board.hpp"
#include "input/BoardMapper.hpp"

TEST_CASE("Pixel maps to correct cell", "[boardmapper]") {
    Board board(8,8);

    auto cell = BoardMapper::pixelToCell(150, 250, board);
    REQUIRE(cell.has_value());
    CHECK(cell->row == 2);
    CHECK(cell->col == 1);
}

TEST_CASE("Click outside the board returns nullopt", "[boardmapper]") {
    Board board(3,3);

    CHECK(BoardMapper::pixelToCell(1000, 1000, board).has_value() == false);
    CHECK(BoardMapper::pixelToCell(-5, 10, board).has_value() == false);
}