#include "MovesLogRenderer.hpp"
#include <sstream>
#include <iomanip>

MovesLogRenderer::MovesLogRenderer(int width, int height, int boardRows, int boardCols)
    : width_(width), height_(height), boardRows_(boardRows), boardCols_(boardCols) {}

std::string MovesLogRenderer::formatTime(long timestampMs) const
{
    long totalSec = timestampMs / 1000;
    long mm = totalSec / 60;
    long ss = totalSec % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << mm << ":"
        << std::setw(2) << std::setfill('0') << ss;
    return oss.str();
}

std::string MovesLogRenderer::squareNotation(Position pos) const
{
    char file = static_cast<char>('a' + pos.col);
    int rank = boardRows_ - pos.row;   
    return std::string(1, file) + std::to_string(rank);
}

std::string MovesLogRenderer::pieceCode(Piece::Color color, Piece::Type type) const
{
    std::string code;
    switch (type)
    {
        case Piece::Type::KING:   code = "K"; break;
        case Piece::Type::QUEEN:  code = "Q"; break;
        case Piece::Type::ROOK:   code = "R"; break;
        case Piece::Type::BISHOP: code = "B"; break;
        case Piece::Type::KNIGHT: code = "N"; break;
        case Piece::Type::PAWN:   code = "P"; break;
    }
    std::string colorCode = (color == Piece::Color::WHITE) ? "W" : "B";
    return colorCode + code;
}


cv::Mat MovesLogRenderer::renderColumn(const GameStats& stats, Piece::Color color) const
{
    cv::Mat canvas(height_, width_, CV_8UC3, cv::Scalar(255, 255, 255));

    std::string title = (color == Piece::Color::WHITE) ? "White" : "Black";
    cv::putText(canvas, title, cv::Point(15, 25),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 0), 2);

    std::string scoreStr = "Score: " + std::to_string(stats.score(color));
    cv::putText(canvas, scoreStr, cv::Point(15, headerHeight_ + 5),
                cv::FONT_HERSHEY_SIMPLEX, 0.55, cv::Scalar(0, 100, 0), 2);

    int y = headerHeight_ + scoreHeight_;

    for (const auto& mv : stats.moves())
    {
        if (mv.color != color) continue;

        std::string line = formatTime(mv.timestampMs) + "  " +
                            pieceCode(mv.color, mv.pieceType) + "  " +
                            squareNotation(mv.from) + "->" + squareNotation(mv.to) +
                            (mv.isJump ? " (jump)" : "");

        if (y + rowHeight_ < height_)
        {
            cv::putText(canvas, line, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, 0.45, cv::Scalar(0, 0, 0), 1);
            y += rowHeight_;
        }
    }

    return canvas;
}