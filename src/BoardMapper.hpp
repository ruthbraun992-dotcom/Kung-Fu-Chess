
// BoardMapper.hpp
#pragma once
#include "Board.hpp"
#include "position.hpp"
#include <optional>

namespace BoardMapper {
    constexpr int CELL_SIZE = 100;
    std::optional<Position> pixelToCell(int x, int y, const Board& board);
}