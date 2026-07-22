#include "AnimationConfigLoader.hpp"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

AnimationConfigLoader::AnimationConfigLoader(const std::string& assetDir)
    : assetDir_(assetDir)
{
    loadAll();
}

std::string AnimationConfigLoader::fileName(Piece::Color color, Piece::Type type) const
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

std::string AnimationConfigLoader::makeKey(Piece::Color color, Piece::Type type, PieceState state) const
{
    return std::to_string(static_cast<int>(color)) + "_" +
           std::to_string(static_cast<int>(type)) + "_" +
           stateToString(state);
}

void AnimationConfigLoader::loadAll()
{
    static const std::vector<PieceState> states =
    {
        PieceState::IDLE, PieceState::MOVE, PieceState::JUMP,
        PieceState::SHORT_REST, PieceState::LONG_REST
    };

    for (int c = 0; c < 2; ++c)
    {
        Piece::Color color = static_cast<Piece::Color>(c);
        for (int t = 0; t < 6; ++t)
        {
            Piece::Type type = static_cast<Piece::Type>(t);
            for (PieceState state : states)
            {
                std::filesystem::path folder =
                    std::filesystem::path(assetDir_) /
                    fileName(color, type) /
                    "states" /
                    stateToString(state) /
                    "sprites";

                if (!std::filesystem::exists(folder))
                    continue;

                int frameCount = 0;
                for (const auto& f : std::filesystem::directory_iterator(folder))
                    if (f.is_regular_file()) ++frameCount;

                if (frameCount == 0)
                    continue;

                AnimationConfig cfg;
                cfg.frameCount = frameCount;

                std::filesystem::path configPath = folder.parent_path() / "config.json";
                if (std::filesystem::exists(configPath))
                {
                    std::ifstream in(configPath);
                    json j;
                    in >> j;

                    if (j.contains("graphics"))
                    {
                        cfg.framesPerSecond = j["graphics"].value("frames_per_sec", 8);
                        cfg.isLoop          = j["graphics"].value("is_loop", true);
                    }
                    if (j.contains("physics"))
                    {
                        cfg.speedMetersPerSec = j["physics"].value("speed_m_per_sec", 0.0);
                        std::string nextStr = j["physics"].value("next_state_when_finished", "idle");
                        cfg.nextStateWhenFinished = stringToState(nextStr);
                    }
                }

                configs_[makeKey(color, type, state)] = cfg;
            }
        }
    }
}

const AnimationConfig& AnimationConfigLoader::get(
    Piece::Color color, Piece::Type type, PieceState state) const
{
    auto it = configs_.find(makeKey(color, type, state));
    if (it == configs_.end())
        throw std::runtime_error("Missing animation config: " + makeKey(color, type, state));
    return it->second;
}