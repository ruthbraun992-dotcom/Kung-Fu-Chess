#include "BoardRenderer.hpp"

BoardRenderer::BoardRenderer(int rows, int cols, int cellSize,
                              const std::string& assetDir,
                              int offsetX, int offsetY)
    : rows_(rows),
      cols_(cols),
      cellSize_(cellSize),
      offsetX_(offsetX),
      offsetY_(offsetY),
      spriteManager_(assetDir)
{
}
void BoardRenderer::setSelectedCell(const std::optional<Position>& selected)
{
    selectedCell_ = selected;
}

void BoardRenderer::draw(const cv::Mat& boardImage, const Board& board, cv::Mat& out) const
{
    if (boardImage.empty())
        return;

    out = boardImage.clone();

    for (int row = 0; row < rows_; ++row)
    {
        for (int col = 0; col < cols_; ++col)
        {
            auto piece = board.getCell(row, col);
            if (!piece.has_value())
                continue;

            const cv::Mat& sprite =
                spriteManager_.getSprite(piece->color(), piece->type());

            int x = offsetX_ + col * cellSize_;
            int y = offsetY_ + row * cellSize_;

            drawSprite(out, sprite, x, y);
        }
    }

    if (selectedCell_.has_value())
    {
        int x = offsetX_ + selectedCell_->col * cellSize_;
        int y = offsetY_ + selectedCell_->row * cellSize_;

        cv::rectangle(out,
                      cv::Rect(x + 2, y + 2, cellSize_ - 4, cellSize_ - 4),
                      cv::Scalar(0, 255, 0),
                      2);
    }
}

void BoardRenderer::drawSprite(cv::Mat& canvas,
                               const cv::Mat& sprite,
                               int x,
                               int y) const
{
    if (sprite.empty())
        return;

    cv::Mat resized;
    cv::resize(sprite, resized, cv::Size(cellSize_, cellSize_));

    cv::Mat roi = canvas(cv::Rect(x, y, cellSize_, cellSize_));

    if (resized.channels() == 4)
    {
        for (int row = 0; row < resized.rows; row++)
        {
            for (int col = 0; col < resized.cols; col++)
            {
                cv::Vec4b pixel = resized.at<cv::Vec4b>(row, col);

                uchar alpha = pixel[3];

                if (alpha == 0)
                    continue;

                if (alpha == 255)
                {
                    roi.at<cv::Vec3b>(row, col) =
                    {
                        pixel[0],
                        pixel[1],
                        pixel[2]
                    };
                }
                else
                {
                    cv::Vec3b& dst = roi.at<cv::Vec3b>(row, col);

                    for (int c = 0; c < 3; c++)
                    {
                        dst[c] =
                            (pixel[c] * alpha +
                             dst[c] * (255 - alpha)) / 255;
                    }
                }
            }
        }
    }
    else
    {
        resized.copyTo(roi);
    }
}