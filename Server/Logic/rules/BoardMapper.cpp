// BoardMapper.cpp
#include "BoardMapper.hpp"
namespace BoardMapper {

std::optional<Position> pixelToCell(int x, int y, const Board& board) {
    if (x < 0 || y < 0) return std::nullopt;

    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;

    if (row < 0 || row >= board.rows() || col < 0 || col >= board.cols()) {
        return std::nullopt;
    }
    return Position{row, col};
}

}