// Server/RoomManager.hpp
#pragma once
#include <map>
#include <string>
#include <vector>
#include <optional>

struct RoomSession {
    std::string roomId;
    int whiteSessionId = -1;
    int blackSessionId = -1;
    std::string whiteUsername;
    std::string blackUsername;
    std::vector<int> viewerSessionIds;   // כל מי שנכנס אחרי שני הראשונים
};

class RoomManager {
public:
    std::string createRoom(int sessionId, const std::string& username);
    
    enum class JoinResult { JoinedAsBlack, JoinedAsViewer, RoomNotFound };
    JoinResult joinRoom(const std::string& roomId, int sessionId, const std::string& username);
    
    void cancelRoom(const std::string& roomId, int sessionId);   // רק יוצר החדר יכול לבטל
    RoomSession* getRoom(const std::string& roomId);
    std::optional<std::string> getRoomIdForSession(int sessionId) const;
    void removeSession(int sessionId);   // לניתוק

private:
    std::string generateRoomId();
    
    int nextRoomId_ = 1;
    std::map<std::string, RoomSession> rooms_;
    std::map<int, std::string> sessionToRoom_;
};