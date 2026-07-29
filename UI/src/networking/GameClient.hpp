#pragma once
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

using json = nlohmann::json;

class GameClient {
public:
    using WsClient = websocketpp::client<websocketpp::config::asio_client>;
    using ConnectionHdl = websocketpp::connection_hdl;

    GameClient();
    
    bool connect(const std::string& uri);
    void disconnect();
    
    void registerUser(const std::string& username, const std::string& password);
    void login(const std::string& username, const std::string& password);
    void createRoom();
    void joinRoom(const std::string& roomId);
    void sendMove(int fromRow, int fromCol, int toRow, int toCol);
    
    // Callbacks
    std::function<void(const json&)> onMessageReceived;
    std::function<void()> onConnect;
    std::function<void()> onDisconnect;
    std::function<void(const json&)> onBoardStateUpdate;
    std::function<void(const json&)> onPieceMove;

private:
    void onOpen(ConnectionHdl hdl);
    void onClose(ConnectionHdl hdl);
    void onMessage(ConnectionHdl hdl, WsClient::message_ptr msg);
    
    WsClient client_;
    ConnectionHdl connectionHdl_;
    bool connected_;
};