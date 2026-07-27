// Server/UserRepository.cpp
#include "UserRepository.hpp"
#include <stdexcept>
#include <iostream>
UserRepository::UserRepository(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Cannot open DB: " + std::string(sqlite3_errmsg(db_)));
    }
    initSchema();
}

UserRepository::~UserRepository() {
    sqlite3_close(db_);
}

void UserRepository::initSchema() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "elo INTEGER NOT NULL DEFAULT 1200"
        ");";
    char* errMsg = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("Schema init failed: " + err);
    }
}

bool UserRepository::createUser(const std::string& username,
                                const std::string& passwordHash)
{
    const char* sql =
        "INSERT INTO users (username, password_hash) VALUES (?, ?);";

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cout << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, passwordHash.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cout << "SQLite error: "
                  << sqlite3_errmsg(db_)
                  << std::endl;
    }

    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}
std::optional<User> UserRepository::findByUsername(const std::string& username) {
    const char* sql = "SELECT id, username, password_hash, elo FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return std::nullopt;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<User> result;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = User{
            sqlite3_column_int(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)),
            sqlite3_column_int(stmt, 3)
        };
    }
    sqlite3_finalize(stmt);
    return result;
}

void UserRepository::updateElo(int userId, int newElo) {
    const char* sql = "UPDATE users SET elo = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return;
    sqlite3_bind_int(stmt, 1, newElo);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}