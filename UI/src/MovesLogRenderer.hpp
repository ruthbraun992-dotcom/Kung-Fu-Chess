#pragma once

#include <opencv2/opencv.hpp>
#include "GameStats.hpp"

class MovesLogRenderer
{
public:
    MovesLogRenderer(int width, int height, int boardRows, int boardCols);

    cv::Mat render(const GameStats& stats) const;

private:
    std::string formatTime(long timestampMs) const;
    std::string squareNotation(Position pos) const;
    std::string pieceCode(Piece::Color color, Piece::Type type) const;

    int width_;
    int height_;
    int boardRows_;
    int boardCols_;
    int rowHeight_ = 22;
    int headerHeight_ = 40;
};