#pragma once

#include <optional>
#include "Position.hpp"

class ClickTranslator {
public:
    ClickTranslator(int rows,
                int cols,
                int cellSize,
                int offsetX = 0,
                int offsetY = 0)
    : rows_(rows),
      cols_(cols),
      cellSize_(cellSize),
      offsetX_(offsetX),
      offsetY_(offsetY) {}

    std::optional<Position> pixelToCell(int x, int y) const;

private:
    int rows_;
    int cols_;
    int cellSize_;

    int offsetX_;
    int offsetY_;
};