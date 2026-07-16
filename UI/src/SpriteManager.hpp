#pragma once

#include <opencv2/opencv.hpp>
#include <map>
#include <string>
#include "Piece.hpp"

class SpriteManager
{
public:
    explicit SpriteManager(const std::string& assetDir = "sprites/");

    const cv::Mat& getSprite(Piece::Color color, Piece::Type type) const;
    std::string spritePath(Piece::Color color, Piece::Type type) const;

    private:
    std::map<std::string, cv::Mat> sprites_;
    std::string assetDir_;

    std::string makeKey(Piece::Color color, Piece::Type type) const;
    std::string fileName(Piece::Color color, Piece::Type type) const;
    void loadAll();
};