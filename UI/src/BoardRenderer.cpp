#include "BoardRenderer.hpp"
#include "PieceState.hpp"
#include <iostream>
BoardRenderer::BoardRenderer(int rows, int cols, int cellSize,
                              const SpriteManager& sprites,
                              int offsetX, int offsetY, const GameEngine& engine)
: rows_(rows), cols_(cols), cellSize_(cellSize),
  offsetX_(offsetX), offsetY_(offsetY),
  engine_(engine), sprites_(sprites) {}

void BoardRenderer::setSelectedCell(const std::optional<Position>& selected)
{
    selectedCell_ = selected;
}

void BoardRenderer::draw(const cv::Mat& boardImage, const Board& board, cv::Mat& out) const
{
    if (boardImage.empty()) return;
out = cv::Mat(800, 800, CV_8UC3, cv::Scalar(255, 255, 255));

boardImage.copyTo(
    out(cv::Rect(
        offsetX_,
        offsetY_,
        boardImage.cols,
        boardImage.rows)));
    for (int row = 0; row < rows_; row++)
    {
        for (int col = 0; col < cols_; col++)
        {
            auto piece = board.getCell(row, col);
            if (!piece.has_value()) continue;

            // המצב האמיתי של הכלי (idle/move/jump/attack/rest) - לא קשיח
            auto activeState = engine_.currentStateOf({row, col});
            PieceState renderState = activeState.value_or(piece->state());

            const cv::Mat& sprite = sprites_.getSprite(
                piece->color(),
                piece->type(),
                stateToString(renderState),
                engine_.currentTime());

            double pixelX, pixelY;
            auto renderPos = engine_.currentPositionOf({row, col});

            if (renderPos.has_value())
            {
                pixelX = offsetX_ + renderPos->col * cellSize_;
                pixelY = offsetY_ + renderPos->row * cellSize_;
            }

            else
            {
                pixelX = offsetX_ + col * cellSize_;
                pixelY = offsetY_ + row * cellSize_;
            }
            if (pixelX < 0 || pixelY < 0 || pixelX + cellSize_ > 800 || pixelY + cellSize_ > 800)
{
    std::cout << "SUSPECT piece at board(" << row << "," << col << ")"
               << " renderPos.has_value=" << renderPos.has_value()
               << " pixelX=" << pixelX << " pixelY=" << pixelY
               << std::endl;
}
            drawSprite(out, sprite, pixelX, pixelY);
        }
    }

    if (selectedCell_.has_value())
    {
        int x = offsetX_ + selectedCell_->col * cellSize_;
        int y = offsetY_ + selectedCell_->row * cellSize_;

        cv::rectangle(out, cv::Rect(x + 2, y + 2, cellSize_ - 4, cellSize_ - 4), cv::Scalar(0, 255, 0), 2);
    }
    // אותיות A-H מעל הלוח
for (int col = 0; col < cols_; ++col)
{
    std::string letter(1, 'A' + col);

    int x = offsetX_ + col * cellSize_ + cellSize_ / 2 - 8;
    int y = offsetY_ - 12;

    cv::putText(out,
                letter,
                cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(0, 0, 0),
                2);
}

// מספרים 8-1 משמאל ללוח
for (int row = 0; row < rows_; ++row)
{
    std::string number = std::to_string(rows_ - row);

    int x = offsetX_ - 22;
    int y = offsetY_ + row * cellSize_ + cellSize_ / 2 + 8;

    cv::putText(out,
                number,
                cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(0, 0, 0),
                2);
}
int whiteScore = engine_.stats().score(Piece::Color::WHITE);
int blackScore = engine_.stats().score(Piece::Color::BLACK);

cv::putText(
    out,
    "White: " + std::to_string(whiteScore),
    cv::Point(10, 30),
    cv::FONT_HERSHEY_SIMPLEX,
    0.7,
    cv::Scalar(0,0,0),
    2);

cv::putText(
    out,
    "Black: " + std::to_string(blackScore),
    cv::Point(10, 60),
    cv::FONT_HERSHEY_SIMPLEX,
    0.7,
    cv::Scalar(0,0,0),
    2);
}

void BoardRenderer::drawSprite(cv::Mat& canvas, const cv::Mat& sprite, double x, double y) const
{
    if (sprite.empty()) return;

    int ix = (int)x;
    int iy = (int)y;

    // הגנה: אם החישוב יצא מחוץ לתמונה (באג במקום אחר), נדלג במקום לקרוס
    if (ix < 0 || iy < 0 || ix + cellSize_ > canvas.cols || iy + cellSize_ > canvas.rows)
    {
        std::cout << "OUT OF BOUNDS drawSprite: x=" << x << " y=" << y
                   << " ix=" << ix << " iy=" << iy
                   << " canvas=" << canvas.cols << "x" << canvas.rows
                   << std::endl;
        return;
    }

    cv::Mat resized;
    cv::resize(sprite, resized, cv::Size(cellSize_, cellSize_));

    cv::Mat roi = canvas(cv::Rect(ix, iy, cellSize_, cellSize_));
    if (resized.channels() == 4)
    {
        for (int r = 0; r < resized.rows; r++)
        {
            for (int c = 0; c < resized.cols; c++)
            {
                cv::Vec4b pixel = resized.at<cv::Vec4b>(r, c);
                uchar alpha = pixel[3];

                if (alpha == 0) continue;

                if (alpha == 255)
                {
                    roi.at<cv::Vec3b>(r, c) = {pixel[0], pixel[1], pixel[2]};
                }
                else
                {
                    cv::Vec3b& dst = roi.at<cv::Vec3b>(r, c);
                    for (int k = 0; k < 3; k++)
                        dst[k] = (pixel[k] * alpha + dst[k] * (255 - alpha)) / 255;
                }
            }
        }
    }
    else
    {
        resized.copyTo(roi);
    }
}
