#pragma once
#include <map>
#include <vector>
#include <string>
#include "Piece.hpp"

class ClientGameStats {
public:
    void updateScore(Piece::Color color, int newScore);
    void addMoveLine(Piece::Color color, const std::string& moveText);

    int score(Piece::Color color) const;
    const std::vector<std::string>& linesFor(Piece::Color color) const;

private:
    std::map<Piece::Color, int> scores_;
    std::map<Piece::Color, std::vector<std::string>> lines_;
    static const std::vector<std::string> empty_;
};