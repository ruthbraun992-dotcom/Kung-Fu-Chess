// Server/MatchmakingQueue.cpp
#include "MatchmakingQueue.hpp"
#include <algorithm>
#include <cmath>

std::optional<std::pair<WaitingPlayer, WaitingPlayer>>
MatchmakingQueue::addPlayer(const WaitingPlayer& player)
{
    for (auto it = waiting_.begin(); it != waiting_.end(); ++it) {
        if (std::abs(it->elo - player.elo) <= kEloRange) {
            WaitingPlayer opponent = *it;
            waiting_.erase(it);
            return std::make_pair(player, opponent);
        }
    }

    waiting_.push_back(player);
    return std::nullopt;
}

void MatchmakingQueue::removePlayer(int sessionId)
{
    waiting_.erase(
        std::remove_if(waiting_.begin(), waiting_.end(),
            [sessionId](const WaitingPlayer& p) { return p.sessionId == sessionId; }),
        waiting_.end());
}