#pragma once
#include <map>
#include <memory>
#include <functional>
#include <string>
#include <optional>
#include <mutex>
#include "GameEngine.hpp"
#include "EventBus.hpp"
#include "Piece.hpp"

struct GameSession {
    std::string gameId;
    std::unique_ptr<EventBus> bus;
    std::unique_ptr<GameEngine> engine;
    int whiteSessionId;
    int blackSessionId;
    std::string whiteUsername;   // חדש - נדרש כדי לזהות שחקן חוזר אחרי ניתוק
    std::string blackUsername;
};

class GameManager {
public:
    using SendFn = std::function<void(int, const std::string&)>;

    struct ReconnectInfo {
        std::string gameId;
        int opponentSessionId;
        Piece::Color reconnectedColor;
    };

    std::string createGame(int whiteSessionId, int blackSessionId,
                            const std::string& whiteUsername, const std::string& blackUsername,
                            SendFn sendToSession);
    GameEngine* getEngineForSession(int sessionId);
    EventBus* getBusForSession(int sessionId);
    std::optional<std::string> getGameIdForSession(int sessionId) const;
    std::optional<int> getOpponentSessionId(int sessionId) const;
    void endGame(const std::string& gameId);

    std::optional<ReconnectInfo> tryReconnect(const std::string& username, int newSessionId);
    void forfeitByUsername(const std::string& gameId, const std::string& disconnectedUsername);
    std::optional<Piece::Color> getPlayerColor(int sessionId) const;
    void update(long ms);
    std::vector<std::string> getAllGameIds() const;
    void GameManager::broadcastBoardState(const std::string& gameId, const SendFn sendToSession);
    json GameManager::createBoardState(const GameEngine& engine);
    void broadcast(const std::string& gameId, const std::string& message,SendFn sendToSession);
    std::mutex& mutex() { return mutex_; }

private:
    void registerEventHandlers(const std::string& gameId, const SendFn& sendToSession);
    void forceGameOver(const std::string& gameId, Piece::Color winner, const std::string& reason);
    std::optional<std::string> findGameIdByUsername(const std::string& username) const;

    int nextGameId_ = 1;
    std::map<std::string, std::unique_ptr<GameSession>> games_;
    std::map<int, std::string> sessionToGame_;
     std::mutex mutex_;
};