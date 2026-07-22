#pragma once

#include <opencv2/opencv.hpp>
#include <map>
#include <string>
#include <vector>
#include "Piece.hpp"
#include "PieceState.hpp"
#include "animation/AnimationConfigLoader.hpp"

class SpriteManager
{
public:
    explicit SpriteManager(const std::string& assetDir = "sprites/");

    const cv::Mat& getSprite(Piece::Color color, Piece::Type type,
                              const std::string& state, long now) const;
    std::string spritePath(Piece::Color color, Piece::Type type) const;

private:
    std::map<std::string, std::vector<cv::Mat>> frames_;
    std::string assetDir_;
    AnimationConfigLoader configs_;

    void loadAll();
    std::string makeKey(Piece::Color color, Piece::Type type, const std::string& state) const;
    std::string fileName(Piece::Color color, Piece::Type type) const;
};