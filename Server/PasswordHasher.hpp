// Server/PasswordHasher.hpp
#pragma once
#include <string>

class PasswordHasher {
public:
    // מחזיר "salt$hash" - שניהם hex strings
    static std::string hashPassword(const std::string& plainPassword);
    // בודק אם plainPassword תואם ל-storedHash (שנשמר כ"salt$hash")
    static bool verifyPassword(const std::string& plainPassword, const std::string& storedHash);

private:
    static std::string generateSalt();
    static std::string sha256(const std::string& input);
};