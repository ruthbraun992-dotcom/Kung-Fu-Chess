#include "ClickTranslator.hpp"
#include <iostream>
#include "Position.hpp"

std::optional<Position> ClickTranslator::pixelToCell(int x, int y) const
{
    const int localX = x - offsetX_;
    const int localY = y - offsetY_;

    if (localX < 0 || localY < 0)
        return std::nullopt;

    const int boardWidth = cols_ * cellSize_;
    const int boardHeight = rows_ * cellSize_;

    if (localX >= boardWidth || localY >= boardHeight)
        return std::nullopt;

    const int col = localX / cellSize_;
    const int row = localY / cellSize_;

    if (row >= rows_ || col >= cols_)
        return std::nullopt;

    return Position{row, col};
}