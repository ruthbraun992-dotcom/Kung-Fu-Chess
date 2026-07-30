#include "ClientGameStats.hpp"

const std::vector<std::string> ClientGameStats::empty_;

void ClientGameStats::updateScore(Piece::Color color, int newScore) {
    scores_[color] = newScore;
}

void ClientGameStats::addMoveLine(Piece::Color color, const std::string& moveText) {
    lines_[color].push_back(moveText);
}

int ClientGameStats::score(Piece::Color color) const {
    auto it = scores_.find(color);
    return (it != scores_.end()) ? it->second : 0;
}

const std::vector<std::string>& ClientGameStats::linesFor(Piece::Color color) const {
    auto it = lines_.find(color);
    return (it != lines_.end()) ? it->second : empty_;
}