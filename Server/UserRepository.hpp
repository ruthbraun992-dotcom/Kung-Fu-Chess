// Server/UserRepository.hpp
#pragma once
#include <sqlite3.h>
#include <optional>
#include <string>

struct User {
    int id;
    std::string username;
    std::string passwordHash;
    int elo;
};

class UserRepository {
public:
    explicit UserRepository(const std::string& dbPath);
    ~UserRepository();

    bool createUser(const std::string& username, const std::string& passwordHash);
    std::optional<User> findByUsername(const std::string& username);
    void updateElo(int userId, int newElo);

private:
    sqlite3* db_;
    void initSchema();
};