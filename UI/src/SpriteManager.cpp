#include "SpriteManager.hpp"

#include <filesystem>
#include <stdexcept>

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
    for (int c = 0; c < 2; ++c)
    {
        Piece::Color color = static_cast<Piece::Color>(c);

        for (int t = 0; t < 6; ++t)
        {
            Piece::Type type = static_cast<Piece::Type>(t);
            std::string key = makeKey(color, type);
            std::string path = spritePath(color, type);

            if (!std::filesystem::exists(path))
            {
                throw std::runtime_error("Sprite file not found: " + path);
            }

            cv::Mat image = cv::imread(path, cv::IMREAD_UNCHANGED);
            if (image.empty())
            {
                throw std::runtime_error("Failed to load sprite: " + path);
            }

            sprites_[key] = std::move(image);
        }
    }
}

const cv::Mat& SpriteManager::getSprite(Piece::Color color, Piece::Type type) const
{
    std::string key = makeKey(color, type);
    auto it = sprites_.find(key);
    if (it == sprites_.end())
    {
        throw std::runtime_error("Sprite not loaded for key: " + key);
    }
    return it->second;
}