#pragma once

#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>

class Img {
public:
    Img();
    
    Img& read(const std::string& path,
              const std::pair<int, int>& size = {},
              bool keep_aspect = false,
              int interpolation = cv::INTER_AREA);
    
    
    void draw_on(Img& other_img, int x, int y);
    void put_text(const std::string& txt, int x, int y, double font_size,
                  const cv::Scalar& color = cv::Scalar(255, 255, 255, 255),
                  int thickness = 1);
    void show();
    const cv::Mat& get_mat() const { return img; }
    bool is_loaded() const { return !img.empty(); }

private:
    cv::Mat img;
}; 