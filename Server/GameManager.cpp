#include "GameManager.hpp"
#include "model/Board.hpp"
#include "Events.hpp"
#include "../UI/src/rendering/BoardSetup.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;

namespace {
    std::string colorToString(Piece::Color c) {
        return c == Piece::Color::WHITE ? "WHITE" : "BLACK";
    }
}

// void GameManager::registerEventHandlers(const std::string& gameId, const SendFn& sendToSession) {
//     auto broadcast = [this, gameId, sendToSession](const json& j) {
//         auto it = games_.find(gameId);
//         if (it == games_.end()) return;
//         std::string payload = j.dump();
//         sendToSession(it->second->whiteSessionId, payload);
//         sendToSession(it->second->blackSessionId, payload);
//     };

//     GameSession& session = *games_.at(gameId);

//     session.bus->subscribe<ScoreUpdatedEvent>([broadcast](const ScoreUpdatedEvent& e) {
//         broadcast(json{ {"type","scoreUpdate"}, {"color", colorToString(e.color)}, {"score", e.newScore} });
//     });

//     session.bus->subscribe<PieceCapturedEvent>([broadcast](const PieceCapturedEvent& e) {
//         broadcast(json{
//             {"type","pieceCaptured"}, {"capturedColor", colorToString(e.capturedColor)},
//             {"pieceType", static_cast<int>(e.pieceType)},
//             {"row", e.atSquare.row}, {"col", e.atSquare.col}
//         });
//     });

//     session.bus->subscribe<MoveLoggedEvent>([this, gameId, sendToSession, broadcast](const MoveLoggedEvent& e)
// {
//     broadcast(json{
//         {"type","moveLogged"},
//         {"color", colorToString(e.color)},
//         {"move", e.moveText}
//     });

//     broadcastBoardState(gameId, sendToSession);
// });
//     session.bus->subscribe<SoundEvent>([broadcast](const SoundEvent& e) {
//         broadcast(json{ {"type","sound"}, {"sound", static_cast<int>(e.type)} });
//     });
// session.bus->subscribe<MotionStartedEvent>(
// [broadcast](const MotionStartedEvent& e)
// {
//     json msg;

//     msg["type"] = "pieceMove";

//     msg["from"] = {
//         {"row", e.from.row},
//         {"col", e.from.col}
//     };

//     msg["to"] = {
//         {"row", e.to.row},
//         {"col", e.to.col}
//     };

//     msg["duration"] = e.durationMs;

//     broadcast(msg);
// });
//     session.bus->subscribe<GameStartedEvent>([broadcast](const GameStartedEvent&) {
//         broadcast(json{ {"type","gameStarted"} });
//     });

//     session.bus->subscribe<GameOverEvent>([this, broadcast, gameId, sendToSession](const GameOverEvent& e) {
//         broadcast(json{ {"type","gameOver"}, {"winner", colorToString(e.winner)}, {"reason", e.reason} });
//         endGame(gameId);
//     });
   
// }

void GameManager::registerEventHandlers(
    const std::string& gameId,
    const SendFn& sendToSession)
{
    static int counter = 0;

    std::cout << "REGISTER EVENT HANDLER #" 
              << ++counter 
              << " FOR GAME "
              << gameId
              << std::endl;
    std::cout << "REGISTER HANDLERS FOR GAME "
          << gameId << std::endl;
    auto broadcast = [this, gameId, sendToSession](const json& j)
    {
        auto it = games_.find(gameId);
        if (it == games_.end())
            return;

        std::string payload = j.dump();

        sendToSession(it->second->whiteSessionId, payload);
        sendToSession(it->second->blackSessionId, payload);
    };


    GameSession& session = *games_.at(gameId);
session.bus->subscribe<MotionFinishedEvent>(
    [this, gameId, sendToSession](const MotionFinishedEvent& e)
    {
        std::cout << "[SERVER] Motion finished - broadcasting board state" << std::endl;

        broadcastBoardState(gameId, sendToSession);
    }
);

    session.bus->subscribe<MotionStartedEvent>(
        [broadcast](const MotionStartedEvent& e)
        {
            json msg;

            msg["type"] = "pieceMove";

            msg["from"] = {
                {"row", e.from.row},
                {"col", e.from.col}
            };

            msg["to"] = {
                {"row", e.to.row},
                {"col", e.to.col}
            };
            msg["piece"] = {
            {"color", colorToString(e.piece.color())},
            {"type", static_cast<int>(e.piece.type())}
        };

            msg["duration"] = e.durationMs;

            broadcast(msg);
        }
    );

    session.bus->subscribe<GameOverEvent>(
        [this, gameId, sendToSession](const GameOverEvent& e)
        {
            std::cout << "🏁 GAME OVER! Winner: " 
                      << (e.winner == Piece::Color::WHITE ? "WHITE" : "BLACK")
                      << " Reason: " << e.reason << std::endl;

            auto it = games_.find(gameId);
            if (it == games_.end()) return;

            json msg{
                {"type", "gameOver"},
                {"winner", e.winner == Piece::Color::WHITE ? "WHITE" : "BLACK"},
                {"reason", e.reason}
            };

            std::string payload = msg.dump();
            sendToSession(it->second->whiteSessionId, payload);
            sendToSession(it->second->blackSessionId, payload);

            endGame(gameId);
        }
    );
    
}
std::string GameManager::createGame(int whiteSessionId, int blackSessionId,
                                     const std::string& whiteUsername, const std::string& blackUsername,
                                     SendFn sendToSession) {
    std::string gameId = std::to_string(nextGameId_++);
std::cout << "CREATE GAME " << gameId << std::endl;
    
    auto session = std::make_unique<GameSession>();
    session->gameId = gameId;
    session->bus = std::make_unique<EventBus>();
    session->whiteSessionId = whiteSessionId;
    session->blackSessionId = blackSessionId;
    session->whiteUsername = whiteUsername;
    session->blackUsername = blackUsername;

    Board board(8, 8);
    setupStartingPosition(board);
    AnimationConfigLoader configs("UI/pieces6");
    session->engine = std::make_unique<GameEngine>(std::move(board), configs, *session->bus);

    games_[gameId] = std::move(session);

sessionToGame_[whiteSessionId] = gameId;
    sessionToGame_[blackSessionId] = gameId;
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

std::optional<Piece::Color> GameManager::getPlayerColor(int sessionId) const
{std::cout << "getPlayerColor sessionId=" << sessionId << std::endl;
std::cout << "sessionToGame size=" << sessionToGame_.size() << std::endl;
    auto it = sessionToGame_.find(sessionId);
    if (it == sessionToGame_.end())
        {
    std::cout << "Session not found!" << std::endl;
    return std::nullopt;
}

    const GameSession& session = *games_.at(it->second);
std::cout << "White session = " << session.whiteSessionId << std::endl;
std::cout << "Black session = " << session.blackSessionId << std::endl;
std::cout << "Current session = " << sessionId << std::endl;
    if (session.whiteSessionId == sessionId)
        return Piece::Color::WHITE;

    if (session.blackSessionId == sessionId)
        return Piece::Color::BLACK;

    return std::nullopt;
}

void GameManager::update(long ms)
{std::cout << "[GameManager::update] Called with ms=" << ms << ", " << games_.size() << " active games" << std::endl;
    
    for (auto& [gameId, game] : games_)
    {
         std::cout << "[GameManager::update] Updating game " << gameId << std::endl;
       game->engine->update(ms);
    }
}

json GameManager::createBoardState(const GameEngine& engine)
{
    json boardState;
    boardState["type"] = "boardState";
    boardState["pieces"] = json::array();

    const Board& board = engine.board();

    for (int row = 0; row < board.rows(); ++row)
    {
        for (int col = 0; col < board.cols(); ++col)
        {
            auto piece = board.getCell(row, col);

            if (!piece.has_value())
                continue;

            boardState["pieces"].push_back({
                {"row", row},
                {"col", col},
                {"color", piece->color() == Piece::Color::WHITE ? "WHITE" : "BLACK"},
                {"type", static_cast<int>(piece->type())}
            });
        }
    }

    return boardState;
}

void GameManager::broadcastBoardState(
    const std::string& gameId,
    SendFn sendToSession)
{std::cout << "אנחנו בתוך broadcastBoardState" << std::endl;
    auto& session = *games_.at(gameId);

    json boardState = createBoardState(*session.engine);

    sendToSession(session.whiteSessionId, boardState.dump());
    sendToSession(session.blackSessionId, boardState.dump());
}
