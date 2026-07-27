// Server/SessionManager.hpp
#pragma once
#include "Piece.hpp"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <map>
#include <optional>
#include <string>

class SessionManager {
public:
    using ConnectionHdl = websocketpp::connection_hdl;

    int registerConnection(ConnectionHdl hdl);
    void setUsername(int sessionId, const std::string& name);
    std::optional<std::string> getUsername(int sessionId) const;
    void removeConnection(int sessionId);
    std::optional<int> findSessionByHandle(ConnectionHdl hdl) const;
    ConnectionHdl getHandle(int sessionId) const;
    std::vector<int> allSessionIds() const;
    std::optional<Piece::Color> assignColor(int sessionId);
    std::optional<Piece::Color> getColor(int sessionId) const;


private:
    int nextSessionId_ = 1;
    std::map<int, ConnectionHdl> handles_;
    std::map<int, std::string>   usernames_;
    std::optional<int> whiteSessionId_;
    std::optional<int> blackSessionId_;
};