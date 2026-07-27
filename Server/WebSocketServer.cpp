// Server/WebSocketServer.cpp
#include "WebSocketServer.hpp"
#include "../Logic/events/EventBus.hpp"
#include "../Logic/events/EventS.hpp"
#include "PasswordHasher.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include "Logger.hpp"

using json = nlohmann::json;

WebSocketServer::WebSocketServer(GameManager& games, UserRepository& users, uint16_t port)
    : games_(games), users_(users), port_(port)
{
    server_.init_asio();
    server_.set_reuse_addr(true);

    server_.set_open_handler([this](ConnectionHdl hdl) { onOpen(hdl); });
    server_.set_close_handler([this](ConnectionHdl hdl) { onClose(hdl); });
    server_.set_message_handler([this](ConnectionHdl hdl, WsServer::message_ptr msg) {
        onMessage(hdl, msg);
    });
}

void WebSocketServer::run() {
    server_.listen(port_);
    server_.start_accept();
    std::cout << "Server listening on port " << port_ << "\n";
    server_.run();   // חוסם - מריץ את ה-io_service/event loop
}

void WebSocketServer::onOpen(ConnectionHdl hdl) {
    int sessionId = sessions_.registerConnection(hdl);
    LOG_EVENT("Client connected, session " + std::to_string(sessionId));
}

void WebSocketServer::onClose(ConnectionHdl hdl) {
      auto sessionId = sessions_.findSessionByHandle(hdl);
    if (!sessionId) return;
    LOG_EVENT("Client disconnected, session " + std::to_string(*sessionId));
    
    std::cout << "Session " << *sessionId << " disconnected\n";

    cancelMatchmakingTimeout(*sessionId);
    matchmaking_.removePlayer(*sessionId);

    auto gameId = games_.getGameIdForSession(*sessionId);
    if (gameId) {
        auto username = sessions_.getUsername(*sessionId);
        if (username) {
            startDisconnectTimeout(*gameId, *sessionId, *username);
        }
    }
    rooms_.removeSession(*sessionId);
    sessions_.removeConnection(*sessionId);
}

void WebSocketServer::onMessage(ConnectionHdl hdl, WsServer::message_ptr msg) {
     auto sessionId = sessions_.findSessionByHandle(hdl);
    if (!sessionId) return;
    LOG_RECV(*sessionId, msg->get_payload());   // <-- לוג כל הודעה נכנסת
    std::string response = handleMessage(*sessionId, msg->get_payload());
    LOG_SEND(*sessionId, response);             // <-- לוג כל תשובה יוצאת
    server_.send(hdl, response, websocketpp::frame::opcode::text);
}
std::string WebSocketServer::handleMessage(int sessionId, const std::string& payload) {
    std::cout << "handleMessage: " << payload << std::endl;
    try {
        json j = json::parse(payload);
        std::string action = j.at("action").get<std::string>();

        if (action == "register") {
            std::string username = j.at("username").get<std::string>();
            std::string password = j.at("password").get<std::string>();
auto existing = users_.findByUsername(username);

std::cout << "Register request for: " << username << std::endl;

if (existing) {
    std::cout << "User already exists in DB!" << std::endl;
} else {
    std::cout << "User does NOT exist." << std::endl;
}
            if (existing) {
    return json{
        {"type", "registerResult"},
        {"success", false},
        {"reason", "username_taken"}
    }.dump();
}

            std::string hash = PasswordHasher::hashPassword(password);
            bool ok = users_.createUser(username, hash);
            return json{ {"type", "registerResult"}, {"success", ok} }.dump();
        }

    if (action == "login") {
    std::string username = j.at("username").get<std::string>();
    std::string password = j.at("password").get<std::string>();

    auto user = users_.findByUsername(username);
    if (!user || !PasswordHasher::verifyPassword(password, user->passwordHash)) {
        return json{ {"type","loginResult"}, {"success", false}, {"reason","invalid_credentials"} }.dump();
    }

    sessions_.setUsername(sessionId, username);

    auto reconnect = games_.tryReconnect(username, sessionId);
    if (reconnect) {
        cancelDisconnectTimeout(reconnect->gameId);
        json toOpponent = { {"type","opponentReconnected"} };
        sendToSession(reconnect->opponentSessionId, toOpponent.dump());

        std::string colorStr = (reconnect->reconnectedColor == Piece::Color::WHITE) ? "WHITE" : "BLACK";
        return json{
            {"type","loginResult"}, {"success", true},
            {"username", username}, {"elo", user->elo}, {"color", colorStr},
            {"reconnected", true}, {"gameId", reconnect->gameId}
        }.dump();
    }

    // אין assignColor - הצבע לא ידוע עדיין, יגיע ב-matchFound
    return json{
        {"type","loginResult"}, {"success", true},
        {"username", username}, {"elo", user->elo}
    }.dump();
}
// WebSocketServer.cpp - move/jump עכשיו דרך games_
if (action == "move") {

    std::cout << "MOVE 1" << std::endl;

    GameEngine* engine = games_.getEngineForSession(sessionId);

    std::cout << "MOVE 2" << std::endl;

    bool ok = engine->requestMove(
        j.at("from").at("row").get<int>(),
        j.at("from").at("col").get<int>(),
        j.at("to").at("row").get<int>(),
        j.at("to").at("col").get<int>()
    );

    std::cout << "MOVE 3 " << ok << std::endl;

    return json{
        {"type","moveResult"},
        {"success",ok}
    }.dump();
}
if (action == "play") {
    auto username = sessions_.getUsername(sessionId);
    auto user = users_.findByUsername(*username);

    WaitingPlayer player{sessionId, user->id, user->username, user->elo};
    auto match = matchmaking_.addPlayer(player);

   if (match) {
    cancelMatchmakingTimeout(match->first.sessionId);
    cancelMatchmakingTimeout(match->second.sessionId);
 std::cout << "First : " << match->first.username
              << " Second: " << match->second.username << std::endl;

    std::string gameId = games_.createGame(
        match->first.sessionId, match->second.sessionId,
        match->first.username, match->second.username,
        [this](int sid, const std::string& msg) { sendToSession(sid, msg); }
    );

    json toSecond = { {"type","matchFound"}, {"gameId", gameId}, {"opponent", match->first.username}, {"color","BLACK"} };
    sendToSession(match->second.sessionId, toSecond.dump());

    json toFirst = { {"type","matchFound"}, {"gameId", gameId}, {"opponent", match->second.username}, {"color","WHITE"} };
    return toFirst.dump();
}

    startMatchmakingTimeout(sessionId);   // לא נמצא match - מתחילים לספור 60 שניות
    return json{ {"type", "searching"} }.dump();
}
    if (action == "createRoom") {
    auto username = sessions_.getUsername(sessionId);
    if (!username) return json{ {"type","error"}, {"message","not_logged_in"} }.dump();

    std::string roomId = rooms_.createRoom(sessionId, *username);
    return json{ {"type","roomCreated"}, {"roomId", roomId} }.dump();
}

if (action == "joinRoom") {
    auto username = sessions_.getUsername(sessionId);
    if (!username) return json{ {"type","error"}, {"message","not_logged_in"} }.dump();

    std::string roomId = j.at("roomId").get<std::string>();
    auto result = rooms_.joinRoom(roomId, sessionId, *username);

    if (result == RoomManager::JoinResult::RoomNotFound) {
        return json{ {"type","joinRoomResult"}, {"success",false}, {"reason","room_not_found"} }.dump();
    }

    if (result == RoomManager::JoinResult::JoinedAsViewer) {
        return json{ {"type","joinRoomResult"}, {"success",true}, {"role","viewer"}, {"roomId",roomId} }.dump();
    }

    // JoinedAsBlack — שני השחקנים מוכנים, מתחילים משחק
    RoomSession* room = rooms_.getRoom(roomId);
    std::string gameId = games_.createGame(
        room->whiteSessionId, room->blackSessionId,
        room->whiteUsername, room->blackUsername,
        [this](int sid, const std::string& msg) { sendToSession(sid, msg); }
    );

    json toWhite = { {"type","gameStarting"}, {"gameId",gameId}, {"color","WHITE"}, {"opponent",room->blackUsername} };
    sendToSession(room->whiteSessionId, toWhite.dump());

    return json{ {"type","gameStarting"}, {"gameId",gameId}, {"color","BLACK"}, {"opponent",room->whiteUsername} }.dump();
}

if (action == "cancelRoom") {
    auto roomIdOpt = rooms_.getRoomIdForSession(sessionId);
    if (!roomIdOpt) return json{ {"type","error"}, {"message","not_in_room"} }.dump();

    RoomSession* room = rooms_.getRoom(*roomIdOpt);
    if (room && room->whiteSessionId != sessionId) {
        return json{ {"type","error"}, {"message","only_creator_can_cancel"} }.dump();
    }

    // נודיע לכל מי שבחדר שהוא בוטל
    if (room) {
        json cancelMsg = { {"type","roomCancelled"}, {"roomId", *roomIdOpt} };
        if (room->blackSessionId != -1) sendToSession(room->blackSessionId, cancelMsg.dump());
        for (int vid : room->viewerSessionIds) sendToSession(vid, cancelMsg.dump());
    }

    rooms_.cancelRoom(*roomIdOpt, sessionId);
    return json{ {"type","cancelRoomResult"}, {"success",true} }.dump();
}
return json{ {"type", "error"}, {"message", "unknown_action"} }.dump();

}
 catch (const json::exception& e) {
        return json{ {"type", "error"}, {"message", e.what()} }.dump();
    }
}


void WebSocketServer::sendToSession(int sessionId, const std::string& payload) {
    try {
        ConnectionHdl hdl = sessions_.getHandle(sessionId);
        LOG_SEND(sessionId, payload);           // <-- לוג גם broadcast
        server_.send(hdl, payload, websocketpp::frame::opcode::text);
    } catch (const std::exception&) {}
}

void WebSocketServer::startMatchmakingTimeout(int sessionId) {
    auto timer = std::make_shared<SteadyTimer>(server_.get_io_service());
    timer->expires_after(std::chrono::seconds(60));

    timer->async_wait([this, sessionId](const std::error_code& ec) {
        if (ec) return; // ec == operation_aborted => הטיימר בוטל כי נמצא match, לא עושים כלום

        matchmaking_.removePlayer(sessionId);
        matchTimers_.erase(sessionId);

        json j = { {"type", "matchNotFound"} };
        sendToSession(sessionId, j.dump());
    });

    matchTimers_[sessionId] = timer;
}

void WebSocketServer::cancelMatchmakingTimeout(int sessionId) {
    auto it = matchTimers_.find(sessionId);
    if (it != matchTimers_.end()) {
        it->second->cancel();   // מפעיל את ה-callback מיד עם ec = operation_aborted
        matchTimers_.erase(it);
    }
}

void WebSocketServer::startDisconnectTimeout(const std::string& gameId, int disconnectedSessionId, const std::string& username) {
    auto opponentId = games_.getOpponentSessionId(disconnectedSessionId);
    if (opponentId) {
        json j = { {"type","opponentDisconnected"}, {"secondsLeft", 20} };
        sendToSession(*opponentId, j.dump());
    }

    auto timer = std::make_shared<SteadyTimer>(server_.get_io_service());
    timer->expires_after(std::chrono::seconds(20));

    timer->async_wait([this, gameId, username](const std::error_code& ec) {
        if (ec) return;   // בוטל - השחקן חזר בזמן
        games_.forfeitByUsername(gameId, username);
        disconnectTimers_.erase(gameId);
    });

    disconnectTimers_[gameId] = { timer, username };
}

void WebSocketServer::cancelDisconnectTimeout(const std::string& gameId) {
    auto it = disconnectTimers_.find(gameId);
    if (it != disconnectTimers_.end()) {
        it->second.timer->cancel();
        disconnectTimers_.erase(it);
    }
}