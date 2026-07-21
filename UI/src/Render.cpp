#include "Render.hpp"
#include "BoardConstants.hpp"

Renderer::Renderer (int rows, int cols, int cellSize, int offsetX, int offsetY)
    : rows_(rows), cols_(cols), cellSize_(cellSize), offsetX_(offsetX), offsetY_(offsetY) {}

void Renderer::setSelectedCell(const std::optional<Position>& selected)
{
    selectedCell_ = selected;
}

void Renderer::render(cv::Mat& canvas) const
{
    if (canvas.empty())
        return;

    if (selectedCell_.has_value())
    {
        int x = offsetX_ + BOARD_MARGIN_X + selectedCell_->col * cellSize_;
        int y = offsetY_ + BOARD_MARGIN_Y + selectedCell_->row * cellSize_;
        cv::Rect roi(x, y, cellSize_, cellSize_);
        cv::rectangle(canvas, roi, cv::Scalar(0, 255, 0), 3);
    }
}