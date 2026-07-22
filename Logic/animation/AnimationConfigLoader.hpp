#pragma once
#include <map>
#include <string>
#include "AnimationConfig.hpp"
#include "Piece.hpp"
#include "PieceState.hpp"

class AnimationConfigLoader
{
public:
    explicit AnimationConfigLoader(const std::string& assetDir);
    const AnimationConfig& get(Piece::Color color, Piece::Type type, PieceState state) const;

private:
    std::map<std::string, AnimationConfig> configs_;
    std::string assetDir_;

    void loadAll();
    std::string makeKey(Piece::Color color, Piece::Type type, PieceState state) const;
    std::string fileName(Piece::Color color, Piece::Type type) const;
};