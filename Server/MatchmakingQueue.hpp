// Server/MatchmakingQueue.hpp
#pragma once
#include <vector>
#include <optional>
#include <utility>
#include <string>

struct WaitingPlayer {
    int sessionId;
    int userId;
    std::string username;
    int elo;
};

class MatchmakingQueue {
public:
    std::optional<std::pair<WaitingPlayer, WaitingPlayer>> addPlayer(const WaitingPlayer& player);
    void removePlayer(int sessionId);

private:
    static constexpr int kEloRange = 100;   // תוקן מ-30 ל-100 לפי המפרט
    std::vector<WaitingPlayer> waiting_;
};