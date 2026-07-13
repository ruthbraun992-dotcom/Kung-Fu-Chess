// Controller.cpp
#include "Controller.hpp"

void Controller::click(int x, int y) {
    auto cellOpt = BoardMapper::pixelToCell(x, y, engine_.board());

    if (!selected_.has_value()) {
        if (!cellOpt.has_value()) return; // outside board, nothing selected: ignore
        if (engine_.board().getCell(cellOpt->row, cellOpt->col).has_value()) {
            selected_ = cellOpt;
        }
        return;
    }

    if (!cellOpt.has_value()) {
        selected_.reset(); // outside board while something selected: cancel
        return;
    }

    Position from = *selected_;
    engine_.requestMove(from.row, from.col, cellOpt->row, cellOpt->col);
    selected_.reset();
}

void Controller::jump(int x, int y) {
    auto cellOpt = BoardMapper::pixelToCell(x, y, engine_.board());
    if (!cellOpt.has_value()) return;
    engine_.requestJump(cellOpt->row, cellOpt->col);
}