#include "SpriteManager.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <stdexcept>

using json = nlohmann::json;

SpriteManager::SpriteManager(const std::string& assetDir)
    : assetDir_(assetDir)
{
    loadAll();
}

std::string SpriteManager::makeKey(Piece::Color color, Piece::Type type) const
{
    return std::to_string(static_cast<int>(color)) + "_" +
           std::to_string(static_cast<int>(type));
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
    std::vector<std::string> states =
    {
        "idle",
        "move",
        "jump",
        "attack",
        "short_rest",
        "long_rest"
    };

    for(int c = 0; c < 2; ++c)
    {
        Piece::Color color =
            static_cast<Piece::Color>(c);

        for(int t = 0; t < 6; ++t)
        {
            Piece::Type type =
                static_cast<Piece::Type>(t);

            for(const std::string& state : states)
            {
                std::string key =
                    makeKey(color, type) + "_" + state;

                std::filesystem::path folder =
                    std::filesystem::path(assetDir_) /
                    fileName(color, type) /
                    "states" /
                    state /
                    "sprites";

                if(!std::filesystem::exists(folder))
                    continue;

                std::vector<cv::Mat> frames;

                for(const auto& file :
                    std::filesystem::directory_iterator(folder))
                {
                    cv::Mat img =
                        cv::imread(file.path().string(),
                                   cv::IMREAD_UNCHANGED);

                    if(!img.empty())
                        frames.push_back(img);
                }

                if(frames.empty())
                    continue;

                std::filesystem::path configPath =
                    folder.parent_path() /
                    "config.json";

                int fps = 8;
                bool loop = true;

                if(std::filesystem::exists(configPath))
                {
                    std::ifstream in(configPath);

                    json j;
                    in >> j;

                    fps =
                        j["graphics"]["frames_per_sec"];

                    loop =
                        j["graphics"]["is_loop"];
                }

                Animation animation;
                animation.frames = std::move(frames);
                animation.framesPerSecond = fps;
                animation.isLoop = loop;

                sprites_[key] = std::move(animation);
            }
        }
    }
}

const cv::Mat&
SpriteManager::getSprite(
    Piece::Color color,
    Piece::Type type,
    const std::string& state,
    long now
) const
{
    std::string key =
        makeKey(color, type) + "_" + state;

    auto it = sprites_.find(key);

    if(it == sprites_.end())
        throw std::runtime_error("Missing animation: " + key);

    const Animation& anim = it->second;

    int frame =
        (now * anim.framesPerSecond) / 1000;

    if(anim.isLoop)
    {
        frame %= anim.frames.size();
    }
    else if(frame >= static_cast<int>(anim.frames.size()))
    {
        frame = static_cast<int>(anim.frames.size()) - 1;
    }

    return anim.frames[frame];
}