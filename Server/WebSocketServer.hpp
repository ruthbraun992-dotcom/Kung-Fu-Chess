#pragma once
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <map>
#include <memory>
#include <string>
#include "GameManager.hpp"
#include "SessionManager.hpp"
#include "UserRepository.hpp"
#include "MatchmakingQueue.hpp"
#include "RoomManager.hpp"


class WebSocketServer {
public:
    using WsServer = websocketpp::server<websocketpp::config::asio>;
    using ConnectionHdl = websocketpp::connection_hdl;
    using SteadyTimer = asio::steady_timer;   // אם המערכת שלך משתמשת ב-Boost.Asio ולא standalone asio, שני זה ל-boost::asio::steady_timer

    WebSocketServer(GameManager& games, UserRepository& users, uint16_t port);

    void run();
    std::string handleMessage(int sessionId, const std::string& payload);
    struct DisconnectTimer {
        std::shared_ptr<SteadyTimer> timer;
        std::string disconnectedUsername;
    };

    std::map<std::string, DisconnectTimer> disconnectTimers_;   // gameId -> טיימר ניתוק פעיל

    void startDisconnectTimeout(const std::string& gameId, int disconnectedSessionId, const std::string& username);
    void cancelDisconnectTimeout(const std::string& gameId);

private:
    void onOpen(ConnectionHdl hdl);
    void onClose(ConnectionHdl hdl);
    void onMessage(ConnectionHdl hdl, WsServer::message_ptr msg);
    void sendToSession(int sessionId, const std::string& payload);

    void startMatchmakingTimeout(int sessionId);
    void cancelMatchmakingTimeout(int sessionId);

    GameManager& games_;
    UserRepository& users_;
    SessionManager sessions_;
    MatchmakingQueue matchmaking_;
    std::map<int, std::shared_ptr<SteadyTimer>> matchTimers_;   // sessionId -> טיימר פעיל בתור

    WsServer server_;
    uint16_t port_;
    RoomManager rooms_;
};