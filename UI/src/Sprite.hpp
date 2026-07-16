#pragma once

#include <opencv2/opencv.hpp>
#include <string>

class Sprite
{
public:
    Sprite() = default;
    Sprite(const std::string& imagePath, int width, int height);

    bool load(const std::string& imagePath, int width, int height);
    void draw(cv::Mat& canvas, int x, int y) const;

    int getWidth() const;
    int getHeight() const;

private:
    std::string imagePath_;
    cv::Mat image_;
    int width_ = 0;
    int height_ = 0;
};
