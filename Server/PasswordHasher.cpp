#include "PasswordHasher.hpp"
#include "picosha2.h"
#include <random>
#include <sstream>
#include <iomanip>

std::string PasswordHasher::generateSalt() {
    std::random_device rd;
    std::stringstream ss;
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (rd() % 256);
    }
    return ss.str();
}

std::string PasswordHasher::sha256(const std::string& input) {
    return picosha2::hash256_hex_string(input);
}

std::string PasswordHasher::hashPassword(const std::string& plainPassword) {
    std::string salt = generateSalt();
    std::string hash = sha256(salt + plainPassword);
    return salt + "$" + hash;
}

bool PasswordHasher::verifyPassword(const std::string& plainPassword, const std::string& storedHash) {
    auto pos = storedHash.find('$');
    if (pos == std::string::npos) return false;
    std::string salt = storedHash.substr(0, pos);
    std::string expectedHash = storedHash.substr(pos + 1);
    return sha256(salt + plainPassword) == expectedHash;
}