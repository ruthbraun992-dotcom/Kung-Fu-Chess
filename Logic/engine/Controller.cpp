// Controller.cpp
#include "Controller.hpp"

void Controller::click(const Position& cell)
{
    if (engine_.isGameOver())
    {
    selected_.reset();
    return;
    }
    if (!selected_.has_value())
    {
        if (engine_.board().getCell(cell.row, cell.col).has_value())
        {
            selected_ = cell;
        }
        return;
    }

    auto selectedPiece = engine_.board().getCell(selected_->row, selected_->col);
    auto targetPiece = engine_.board().getCell(cell.row, cell.col);

    bool clickedDifferentCell =
        !(cell.row == selected_->row &&
          cell.col == selected_->col);

    if (clickedDifferentCell &&
        targetPiece.has_value() &&
        selectedPiece.has_value() &&
        targetPiece->color() == selectedPiece->color())
        {
            selected_ = cell;
            return;
        }

    Position from = *selected_;
    bool moved =engine_.requestMove(
        from.row,
        from.col,
        cell.row,
        cell.col);
        if (engine_.isGameOver())
        {
            selected_.reset();
            return;
        }
    if (moved)
    {
        selected_.reset();
    }
}

void Controller::jump(const Position& cell) {
    bool ok = engine_.requestJump(cell.row, cell.col);
}
