// Server/SessionManager.cpp
#include "SessionManager.hpp"

int SessionManager::registerConnection(ConnectionHdl hdl) {
    int id = nextSessionId_++;
    handles_[id] = hdl;
    return id;
}

void SessionManager::setUsername(int sessionId, const std::string& name) {
    usernames_[sessionId] = name;
}

std::optional<std::string> SessionManager::getUsername(int sessionId) const {
    auto it = usernames_.find(sessionId);
    if (it == usernames_.end()) return std::nullopt;
    return it->second;
}

void SessionManager::removeConnection(int sessionId) {
    handles_.erase(sessionId);
    usernames_.erase(sessionId);
}

std::optional<int> SessionManager::findSessionByHandle(ConnectionHdl hdl) const {
    for (const auto& [id, h] : handles_) {
        if (!h.owner_before(hdl) && !hdl.owner_before(h)) return id;
    }
    return std::nullopt;
}

SessionManager::ConnectionHdl SessionManager::getHandle(int sessionId) const {
    return handles_.at(sessionId);
}

std::vector<int> SessionManager::allSessionIds() const {
    std::vector<int> ids;
    for (const auto& [id, h] : handles_) ids.push_back(id);
    return ids;
}
std::optional<Piece::Color> SessionManager::assignColor(int sessionId) {
    if (!whiteSessionId_.has_value()) {
        whiteSessionId_ = sessionId;
        return Piece::Color::WHITE;
    }
    if (!blackSessionId_.has_value()) {
        blackSessionId_ = sessionId;
        return Piece::Color::BLACK;
    }
    return std::nullopt;   // כבר יש שני שחקנים - שלב 3 תומך רק בשניים
}

std::optional<Piece::Color> SessionManager::getColor(int sessionId) const {
    if (whiteSessionId_ == sessionId) return Piece::Color::WHITE;
    if (blackSessionId_ == sessionId) return Piece::Color::BLACK;
    return std::nullopt;
}