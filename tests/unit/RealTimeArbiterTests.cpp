#include <catch2/catch_test_macros.hpp>
#include "Board.hpp"
#include "Piece.hpp"
#include "RealTimeArbiter.hpp"

TEST_CASE("Motion does not arrive before its scheduled time", "[arbiter]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    RealTimeArbiter arbiter;
    arbiter.startMotion(Motion{
        Position{0,0}, Position{0,2},
        *board.getCell(0,0), 2000
    });

    arbiter.advanceTime(999, board);
    CHECK(board.getCell(0,0).has_value() == true);  // piece still logically at source
    CHECK(board.getCell(0,2).has_value() == false);
}

TEST_CASE("Motion resolves once elapsed time reaches arrival", "[arbiter]") {
    Board board(3,3);
    board.setCell(0,0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    RealTimeArbiter arbiter;
    arbiter.startMotion(Motion{
        Position{0,0}, Position{0,2},
        *board.getCell(0,0), 2000
    });

    arbiter.advanceTime(1000, board);
    arbiter.advanceTime(1000, board);

    CHECK(board.getCell(0,0).has_value() == false);
    CHECK(board.getCell(0,2).has_value() == true);
    CHECK(arbiter.hasActiveMotion() == false);
}