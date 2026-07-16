#pragma once

#include <opencv2/opencv.hpp>
#include <optional>
#include <string>
#include "SpriteManager.hpp"
#include "Board.hpp"
#include "position.hpp"

class BoardRenderer
{
public:
    BoardRenderer(int rows, int cols, int cellSize,
                  const std::string& assetDir,
                  int offsetX = 0, int offsetY = 0);

    void setSelectedCell(const std::optional<Position>& selected);
    void draw(const cv::Mat& boardImage, const Board& board, cv::Mat& out) const;

private:
    int rows_;
    int cols_;
    int cellSize_;
    int offsetX_;
    int offsetY_;
    std::optional<Position> selectedCell_;

    SpriteManager spriteManager_;

    void drawSprite(cv::Mat& canvas, const cv::Mat& sprite, int x, int y) const;
};