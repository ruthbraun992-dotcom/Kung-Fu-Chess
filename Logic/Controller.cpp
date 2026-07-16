// Controller.cpp
#include "Controller.hpp"

void Controller::click(int x, int y) {
    auto cellOpt = BoardMapper::pixelToCell(x, y, engine_.board());

    if (!selected_.has_value()) {
        if (!cellOpt.has_value()) return;
        if (engine_.board().getCell(cellOpt->row, cellOpt->col).has_value()) {
            selected_ = cellOpt;
        }
        return;
    }

    if (!cellOpt.has_value()) {
        selected_.reset();
        return;
    }

    auto selectedPiece = engine_.board().getCell(selected_->row, selected_->col);
    auto targetPiece = engine_.board().getCell(cellOpt->row, cellOpt->col);

    bool clickedDifferentCell = !(cellOpt->row == selected_->row && cellOpt->col == selected_->col);

    if (clickedDifferentCell && targetPiece.has_value() && selectedPiece.has_value()
        && targetPiece->color() == selectedPiece->color()) {
        selected_ = cellOpt; // מחליפים בחירה לכלי הידידותי החדש
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