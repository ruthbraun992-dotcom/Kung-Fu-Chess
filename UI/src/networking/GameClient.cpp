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

// void GameClient::onMessage(ConnectionHdl hdl, WsClient::message_ptr msg) {
//     std::cout << "[UI] 📨 Message received: " << message << std::endl;
//     try {
//         json j = json::parse(msg->get_payload());
//         if (j.value("type", "") == "boardState") {
//             if (onBoardStateUpdate) {
//                 onBoardStateUpdate(j);
//             }
//         }
//         if (onMessageReceived) onMessageReceived(j);
//     } catch (const std::exception& e) {
//         std::cerr << "Message parse error: " << e.what() << std::endl;
//     }
// }

//from GPT
void GameClient::onMessage(ConnectionHdl hdl, WsClient::message_ptr msg)
{
    const std::string& message = msg->get_payload();

    try
    {
        json msgJson = json::parse(message);

        if (msgJson.value("type", "") == "boardState")
        {
            if (onBoardStateUpdate)
            {
                onBoardStateUpdate(msgJson);
            }
        }
    if (msgJson.value("type", "") == "pieceMotion")
    {
        if (onPieceMotion)
        {
            onPieceMotion(msgJson);
        }
    }
    if (msgJson.value("type", "") == "scoreUpdate")
    {
        if (onScoreUpdate)
            onScoreUpdate(msgJson);
    }
if (msgJson.value("type", "") == "moveResult")
{
    if (onMoveResult)
        onMoveResult(msgJson);
}if(msgJson.value("type","")=="jumpResult")
{
    if(onJumpResult)
        onJumpResult(msgJson);
}
if(msgJson.value("type","") == "pieceJump")
{std::cout << "CLIENT GOT PIECE JUMP" << std::endl;
    if(onPieceJump)
        onPieceJump(msgJson);
}
    if (msgJson.value("type", "") == "moveLogged")
    {
        if (onMoveLogged)
            onMoveLogged(msgJson);
    }
        if (onMessageReceived)
        {
            onMessageReceived(msgJson);
        }
    }
    catch (const std::exception& e)
    {
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
void GameClient::sendJump(int row,int col)
{
    json msg{
        {"action","jump"},
        {"position",
            {
                {"row",row},
                {"col",col}
            }
        }
    };

    client_.send(
        connectionHdl_,
        msg.dump(),
        websocketpp::frame::opcode::text
    );
}
