#include "Sprite.hpp"

Sprite::Sprite(const std::string& imagePath, int width, int height)
    : imagePath_(imagePath), width_(width), height_(height)
{
    image_ = cv::imread(imagePath_, cv::IMREAD_UNCHANGED);
}

bool Sprite::load(const std::string& imagePath, int width, int height)
{
    imagePath_ = imagePath;
    width_ = width;
    height_ = height;

    image_ = cv::imread(imagePath_, cv::IMREAD_UNCHANGED);
    return !image_.empty();
}

void Sprite::draw(cv::Mat& canvas, int x, int y) const
{
    if (image_.empty())
        return;

    cv::Mat resized;
    cv::resize(image_, resized, cv::Size(width_, height_));

    cv::Rect roi(x, y, resized.cols, resized.rows);
    if (roi.x + roi.width > canvas.cols)
        roi.width = canvas.cols - roi.x;
    if (roi.y + roi.height > canvas.rows)
        roi.height = canvas.rows - roi.y;

    if (roi.width <= 0 || roi.height <= 0)
        return;

    resized.copyTo(canvas(roi));
}

int Sprite::getWidth() const
{
    return width_;
}

int Sprite::getHeight() const
{
    return height_;
}
