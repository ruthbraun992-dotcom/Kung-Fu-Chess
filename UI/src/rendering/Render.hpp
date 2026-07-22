#pragma once

#include <opencv2/opencv.hpp>
#include <optional>
#include "position.hpp"
class Renderer
{
public:
    Renderer(int rows, int cols, int cellSize, int offsetX = 0, int offsetY = 0);

    void setSelectedCell(const std::optional<Position>& selected);
    void render(cv::Mat& canvas) const;

private:
    int rows_;
    int cols_;
    int cellSize_;
    int offsetX_;
    int offsetY_;
    std::optional<Position> selectedCell_;
};