#include "ClickTranslator.hpp"
#include "Position.hpp"
#include "rendering/BoardConstants.hpp"

std::optional<Position> ClickTranslator::pixelToCell(int x, int y) const
{
    const int boardMarginX = 114;
    const int boardMarginY = 114;

    const int localX = x - offsetX_ - BOARD_MARGIN_X;
    const int localY = y - offsetY_ - BOARD_MARGIN_Y;

    if (localX < 0 || localY < 0)
        return std::nullopt;

    const int boardWidth = cols_ * cellSize_;
    const int boardHeight = rows_ * cellSize_;

    if (localX >= boardWidth || localY >= boardHeight)
        return std::nullopt;

    const int col = localX / cellSize_;
    const int row = localY / cellSize_;

    return Position{row, col};
}