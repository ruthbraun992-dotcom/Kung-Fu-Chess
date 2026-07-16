#pragma once

#include <opencv2/opencv.hpp>
#include <optional>
#include <string>
#include "SpriteManager.hpp"
#include "Board.hpp"
#include "position.hpp"
#include "GameEngine.hpp"
#include "RenderPosition.hpp"

class BoardRenderer
{
public:
    BoardRenderer(int rows, int cols, int cellSize,
                  const std::string& assetDir,
                  int offsetX, int offsetY, const GameEngine& engine);

    void setSelectedCell(const std::optional<Position>& selected);
    void draw(const cv::Mat& boardImage,const Board& board, cv::Mat& out) const;

private:
    int rows_;
    int cols_;
    int cellSize_;
    int offsetX_;
    int offsetY_;
    const GameEngine& engine_;

    std::optional<Position> selectedCell_;

    SpriteManager spriteManager_;

    void drawSprite(cv::Mat& canvas, const cv::Mat& sprite, double x, double y) const;
};