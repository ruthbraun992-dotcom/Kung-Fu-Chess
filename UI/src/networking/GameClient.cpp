#include "GameClient.hpp"
#include <iostream>

GameClient::GameClient() : connected_(false) {}

bool GameClient::connect(const std::string& uri) {
    try {
        client_.init_asio();
        client_.set_open_handler([this](ConnectionHdl hdl) { onOpen(hdl); });
        client_.set_close_handler([this](ConnectionHdl hdl) { onClose(hdl); });
        client_.set_message_handler([this](ConnectionHdl hdl, WsClient::message_ptr msg) {
            onMessage(hdl, msg);
        });

        websocketpp::lib::error_code ec;
        WsClient::connection_ptr con = client_.get_connection(uri, ec);
        if (ec) {
            std::cerr << "Connect error: " << ec.message() << std::endl;
            return false;
        }

        connectionHdl_ = con->get_handle();
        client_.connect(con);

        std::thread(&WsClient::run, &client_).detach();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return false;
    }
}

void GameClient::onOpen(ConnectionHdl hdl) {
    connected_ = true;
    if (onConnect) onConnect();
}

void GameClient::onClose(ConnectionHdl hdl) {
    connected_ = false;
    if (onDisconnect) onDisconnect();
}

void GameClient::onMessage(ConnectionHdl hdl, WsClient::message_ptr msg) {
    try {
        json j = json::parse(msg->get_payload());
        if (j.value("type", "") == "boardState") {
            if (onBoardStateUpdate) {
                onBoardStateUpdate(j);
            }
        }
        if (onMessageReceived) onMessageReceived(j);
    } catch (const std::exception& e) {
        std::cerr << "Message parse error: " << e.what() << std::endl;
    }
}

void GameClient::registerUser(const std::string& username, const std::string& password) {
    json msg{
        {"action", "register"},
        {"username", username},
        {"password", password}
    };
    client_.send(connectionHdl_, msg.dump(), websocketpp::frame::opcode::text);
}

void GameClient::login(const std::string& username, const std::string& password) {
    json msg{
        {"action", "login"},
        {"username", username},
        {"password", password}
    };
    client_.send(connectionHdl_, msg.dump(), websocketpp::frame::opcode::text);
}

void GameClient::createRoom() {
    json msg{{"action", "createRoom"}};
    client_.send(connectionHdl_, msg.dump(), websocketpp::frame::opcode::text);
}

void GameClient::joinRoom(const std::string& roomId) {
    json msg{
        {"action", "joinRoom"},
        {"roomId", roomId}
    };
    client_.send(connectionHdl_, msg.dump(), websocketpp::frame::opcode::text);
}

void GameClient::sendMove(int fromRow, int fromCol, int toRow, int toCol) {
    json msg{
        {"action", "move"},
        {"from", {{"row", fromRow}, {"col", fromCol}}},
        {"to", {{"row", toRow}, {"col", toCol}}}
    };
    client_.send(connectionHdl_, msg.dump(), websocketpp::frame::opcode::text);
}

void GameClient::disconnect() {
    client_.close(connectionHdl_, websocketpp::close::status::normal, "");
}
