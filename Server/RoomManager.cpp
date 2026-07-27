#include "RoomManager.hpp"
#include <algorithm>

std::string RoomManager::generateRoomId() {
    return "R" + std::to_string(nextRoomId_++);
}

std::string RoomManager::createRoom(int sessionId, const std::string& username) {
    std::string roomId = generateRoomId();
    RoomSession room;
    room.roomId = roomId;
    room.whiteSessionId = sessionId;
    room.whiteUsername = username;
    rooms_[roomId] = room;
    sessionToRoom_[sessionId] = roomId;
    return roomId;
}

RoomManager::JoinResult RoomManager::joinRoom(const std::string& roomId, int sessionId, const std::string& username) {
    auto it = rooms_.find(roomId);
    if (it == rooms_.end()) return JoinResult::RoomNotFound;

    RoomSession& room = it->second;
    sessionToRoom_[sessionId] = roomId;

    if (room.blackSessionId == -1) {
        room.blackSessionId = sessionId;
        room.blackUsername = username;
        return JoinResult::JoinedAsBlack;
    }

    room.viewerSessionIds.push_back(sessionId);
    return JoinResult::JoinedAsViewer;
}

void RoomManager::cancelRoom(const std::string& roomId, int sessionId) {
    auto it = rooms_.find(roomId);
    if (it == rooms_.end()) return;
    if (it->second.whiteSessionId != sessionId) return;   // רק היוצר יכול לבטל

    // מנקים את כל ה-sessions שהיו בחדר
    sessionToRoom_.erase(it->second.whiteSessionId);
    if (it->second.blackSessionId != -1) sessionToRoom_.erase(it->second.blackSessionId);
    for (int vid : it->second.viewerSessionIds) sessionToRoom_.erase(vid);

    rooms_.erase(it);
}

RoomSession* RoomManager::getRoom(const std::string& roomId) {
    auto it = rooms_.find(roomId);
    if (it == rooms_.end()) return nullptr;
    return &it->second;
}

std::optional<std::string> RoomManager::getRoomIdForSession(int sessionId) const {
    auto it = sessionToRoom_.find(sessionId);
    if (it == sessionToRoom_.end()) return std::nullopt;
    return it->second;
}

void RoomManager::removeSession(int sessionId) {
    auto it = sessionToRoom_.find(sessionId);
    if (it == sessionToRoom_.end()) return;
    
    std::string roomId = it->second;
    sessionToRoom_.erase(it);

    auto rit = rooms_.find(roomId);
    if (rit == rooms_.end()) return;

    RoomSession& room = rit->second;
    if (room.whiteSessionId == sessionId) {
        // היוצר ניתק - החדר מתבטל אוטומטית
        if (room.blackSessionId != -1) sessionToRoom_.erase(room.blackSessionId);
        for (int vid : room.viewerSessionIds) sessionToRoom_.erase(vid);
        rooms_.erase(rit);
    } else if (room.blackSessionId == sessionId) {
        room.blackSessionId = -1;
        room.blackUsername = "";
    } else {
        room.viewerSessionIds.erase(
            std::remove(room.viewerSessionIds.begin(), room.viewerSessionIds.end(), sessionId),
            room.viewerSessionIds.end());
    }
}