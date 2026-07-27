#include "GameManager.hpp"
#include "model/Board.hpp"
#include "Events.hpp"
#include "../UI/src/rendering/BoardSetup.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    std::string colorToString(Piece::Color c) {
        return c == Piece::Color::WHITE ? "WHITE" : "BLACK";
    }
}

void GameManager::registerEventHandlers(const std::string& gameId, const SendFn& sendToSession) {
    // broadcast מחפש את ה-sessionId העדכני בכל קריאה - כך שאחרי reconnect
    // (ששינה whiteSessionId/blackSessionId ב-GameSession) האירועים מגיעים למקום הנכון
    auto broadcast = [this, gameId, sendToSession](const json& j) {
        auto it = games_.find(gameId);
        if (it == games_.end()) return;
        std::string payload = j.dump();
        sendToSession(it->second->whiteSessionId, payload);
        sendToSession(it->second->blackSessionId, payload);
    };

    GameSession& session = *games_.at(gameId);

    session.bus->subscribe<ScoreUpdatedEvent>([broadcast](const ScoreUpdatedEvent& e) {
        broadcast(json{ {"type","scoreUpdate"}, {"color", colorToString(e.color)}, {"score", e.newScore} });
    });

    session.bus->subscribe<MoveLoggedEvent>([broadcast](const MoveLoggedEvent& e) {
        broadcast(json{ {"type","moveLogged"}, {"color", colorToString(e.color)}, {"move", e.moveText} });
    });

    session.bus->subscribe<PieceCapturedEvent>([broadcast](const PieceCapturedEvent& e) {
        broadcast(json{
            {"type","pieceCaptured"}, {"capturedColor", colorToString(e.capturedColor)},
            {"pieceType", static_cast<int>(e.pieceType)},
            {"row", e.atSquare.row}, {"col", e.atSquare.col}
        });
    });

    session.bus->subscribe<SoundEvent>([broadcast](const SoundEvent& e) {
        broadcast(json{ {"type","sound"}, {"sound", static_cast<int>(e.type)} });
    });

    session.bus->subscribe<GameStartedEvent>([broadcast](const GameStartedEvent&) {
        broadcast(json{ {"type","gameStarted"} });
    });

    session.bus->subscribe<GameOverEvent>([this, broadcast, gameId](const GameOverEvent& e) {
        broadcast(json{ {"type","gameOver"}, {"winner", colorToString(e.winner)}, {"reason", e.reason} });
        endGame(gameId);
    });
}

std::string GameManager::createGame(int whiteSessionId, int blackSessionId,
                                     const std::string& whiteUsername, const std::string& blackUsername,
                                     SendFn sendToSession) {
    std::string gameId = std::to_string(nextGameId_++);

    auto session = std::make_unique<GameSession>();
    session->gameId = gameId;
    session->bus = std::make_unique<EventBus>();
    session->whiteSessionId = whiteSessionId;
    session->blackSessionId = blackSessionId;
    session->whiteUsername = whiteUsername;
    session->blackUsername = blackUsername;

    Board board(8, 8);
    setupStartingPosition(board);
    AnimationConfigLoader configs("C:\\Users\\This User\\Desktop\\Kung-Fu Chess\\UI\\pieces6");
    session->engine = std::make_unique<GameEngine>(std::move(board), configs, *session->bus);

    sessionToGame_[whiteSessionId] = gameId;
    sessionToGame_[blackSessionId] = gameId;
    games_[gameId] = std::move(session);

    registerEventHandlers(gameId, sendToSession);   // אחרי ההוספה ל-games_, כי הפונקציה עושה games_.at(gameId)

    return gameId;
}

GameEngine* GameManager::getEngineForSession(int sessionId) {
    auto it = sessionToGame_.find(sessionId);
    if (it == sessionToGame_.end()) return nullptr;
    return games_.at(it->second)->engine.get();
}

EventBus* GameManager::getBusForSession(int sessionId) {
    auto it = sessionToGame_.find(sessionId);
    if (it == sessionToGame_.end()) return nullptr;
    return games_.at(it->second)->bus.get();
}

std::optional<std::string> GameManager::getGameIdForSession(int sessionId) const {
    auto it = sessionToGame_.find(sessionId);
    if (it == sessionToGame_.end()) return std::nullopt;
    return it->second;
}

std::optional<int> GameManager::getOpponentSessionId(int sessionId) const {
    auto it = sessionToGame_.find(sessionId);
    if (it == sessionToGame_.end()) return std::nullopt;
    const auto& session = games_.at(it->second);
    if (session->whiteSessionId == sessionId) return session->blackSessionId;
    return session->whiteSessionId;
}

std::optional<std::string> GameManager::findGameIdByUsername(const std::string& username) const {
    for (const auto& [gameId, session] : games_) {
        if (session->whiteUsername == username || session->blackUsername == username) {
            return gameId;
        }
    }
    return std::nullopt;
}

std::optional<GameManager::ReconnectInfo> GameManager::tryReconnect(const std::string& username, int newSessionId) {
    auto gameIdOpt = findGameIdByUsername(username);
    if (!gameIdOpt) return std::nullopt;

    GameSession& session = *games_.at(*gameIdOpt);
    Piece::Color color;
    int oldSessionId;
    int opponentSessionId;

    if (session.whiteUsername == username) {
        oldSessionId = session.whiteSessionId;
        session.whiteSessionId = newSessionId;
        color = Piece::Color::WHITE;
        opponentSessionId = session.blackSessionId;
    } else {
        oldSessionId = session.blackSessionId;
        session.blackSessionId = newSessionId;
        color = Piece::Color::BLACK;
        opponentSessionId = session.whiteSessionId;
    }

    sessionToGame_.erase(oldSessionId);
    sessionToGame_[newSessionId] = *gameIdOpt;

    return ReconnectInfo{ *gameIdOpt, opponentSessionId, color };
}

void GameManager::forceGameOver(const std::string& gameId, Piece::Color winner, const std::string& reason) {
    auto it = games_.find(gameId);
    if (it == games_.end()) return;
    it->second->bus->publish(GameOverEvent{ winner, reason });   // מפעיל את ה-subscriber שכבר קיים -> broadcast + endGame
}

void GameManager::forfeitByUsername(const std::string& gameId, const std::string& disconnectedUsername) {
    auto it = games_.find(gameId);
    if (it == games_.end()) return;
    Piece::Color winner = (it->second->whiteUsername == disconnectedUsername)
        ? Piece::Color::BLACK : Piece::Color::WHITE;
    forceGameOver(gameId, winner, "opponent_disconnected");
}

void GameManager::endGame(const std::string& gameId) {
    auto it = games_.find(gameId);
    if (it == games_.end()) return;

    sessionToGame_.erase(it->second->whiteSessionId);
    sessionToGame_.erase(it->second->blackSessionId);
    games_.erase(it);
}