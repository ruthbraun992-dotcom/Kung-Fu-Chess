#include "SpriteManager.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>

SpriteManager::SpriteManager(const std::string& assetDir)
    : assetDir_(assetDir), configs_(assetDir)
{
    loadAll();
}

std::string SpriteManager::fileName(Piece::Color color, Piece::Type type) const
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
    return code + colorCode;
}

std::string SpriteManager::makeKey(Piece::Color color, Piece::Type type, const std::string& state) const
{
    return std::to_string(static_cast<int>(color)) + "_" +
           std::to_string(static_cast<int>(type)) + "_" + state;
}

std::string SpriteManager::spritePath(Piece::Color color, Piece::Type type) const
{
    std::filesystem::path path(assetDir_);
    path /= fileName(color, type);
    path /= "states";
    path /= "idle";
    path /= "sprites";
    path /= "1.png";
    return path.string();
}

void SpriteManager::loadAll()
{
    static const std::vector<std::string> states =
    {
        "idle", "move", "jump", "attack", "short_rest", "long_rest"
    };

    for (int c = 0; c < 2; ++c)
    {
        Piece::Color color = static_cast<Piece::Color>(c);
        for (int t = 0; t < 6; ++t)
        {
            Piece::Type type = static_cast<Piece::Type>(t);

            for (const std::string& state : states)
            {
                std::filesystem::path folder =
                    std::filesystem::path(assetDir_) /
                    fileName(color, type) /
                    "states" /
                    state /
                    "sprites";

                if (!std::filesystem::exists(folder))
                    continue;

                std::vector<std::filesystem::path> framePaths;
                for (const auto& f : std::filesystem::directory_iterator(folder))
                    if (f.is_regular_file()) framePaths.push_back(f.path());
                std::sort(framePaths.begin(), framePaths.end());

                std::vector<cv::Mat> imgs;
                for (const auto& p : framePaths)
                {
                    cv::Mat img = cv::imread(p.string(), cv::IMREAD_UNCHANGED);
                    if (!img.empty())
                        imgs.push_back(std::move(img));
                }

                if (!imgs.empty())
                    frames_[makeKey(color, type, state)] = std::move(imgs);
            }
        }
    }
}

const cv::Mat& SpriteManager::getSprite(
    Piece::Color color, Piece::Type type,
    const std::string& state, long now) const
{
    std::string key = makeKey(color, type, state);
    auto it = frames_.find(key);
    if (it == frames_.end())
        throw std::runtime_error("Missing sprite frames: " + key);

    const auto& cfg = configs_.get(color, type, stringToState(state));
    const auto& frames = it->second;

    int frame = static_cast<int>((now * cfg.framesPerSecond) / 1000);

    if (cfg.isLoop)
        frame %= static_cast<int>(frames.size());
    else if (frame >= static_cast<int>(frames.size()))
        frame = static_cast<int>(frames.size()) - 1;

    return frames[frame];
}