#pragma once

#include <opencv2/opencv.hpp>
#include <map>
#include <string>
#include <vector>
#include "Piece.hpp"

class SpriteManager
{
public:
    explicit SpriteManager(const std::string& assetDir = "sprites/");
    void loadAll();
    const cv::Mat& getSprite(  Piece::Color color, Piece::Type type,  const std::string& state,  long now) const;
    std::string spritePath(Piece::Color color, Piece::Type type) const;

    private:
    struct Animation
    {
    std::vector<cv::Mat> frames;
    int framesPerSecond = 1;
    bool isLoop = true;
    };
    std::map<std::string, Animation> sprites_;    std::string assetDir_;
    
    std::string makeKey(Piece::Color color, Piece::Type type) const;
    std::string fileName(Piece::Color color, Piece::Type type) const;
    
};