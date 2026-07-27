#include <catch2/catch_test_macros.hpp>
#include "../../server/UserRepository.hpp"
#include "../../server/PasswordHasher.hpp"

TEST_CASE("PasswordHasher: verify succeeds with correct password", "[PasswordHasher]") {
    std::string hash = PasswordHasher::hashPassword("secret123");
    REQUIRE(PasswordHasher::verifyPassword("secret123", hash));
}

TEST_CASE("PasswordHasher: verify fails with wrong password", "[PasswordHasher]") {
    std::string hash = PasswordHasher::hashPassword("secret123");
    REQUIRE_FALSE(PasswordHasher::verifyPassword("wrongPassword", hash));
}

TEST_CASE("PasswordHasher: two hashes of same password differ (different salt)", "[PasswordHasher]") {
    std::string hash1 = PasswordHasher::hashPassword("secret123");
    std::string hash2 = PasswordHasher::hashPassword("secret123");
    REQUIRE(hash1 != hash2);
}

TEST_CASE("UserRepository: createUser then findByUsername returns the user", "[UserRepository]") {
    UserRepository repo(":memory:");   // DB זמני בזיכרון - לא נשאר קובץ אחרי הטסט
    repo.createUser("Ruth", "somehash");

    auto user = repo.findByUsername("Ruth");
    REQUIRE(user.has_value());
    REQUIRE(user->username == "Ruth");
    REQUIRE(user->elo == 1200);  // ברירת המחדל מה-schema
}

TEST_CASE("UserRepository: findByUsername returns nullopt for unknown user", "[UserRepository]") {
    UserRepository repo(":memory:");
    auto user = repo.findByUsername("NoSuchUser");
    REQUIRE_FALSE(user.has_value());
}

TEST_CASE("UserRepository: createUser fails on duplicate username", "[UserRepository]") {
    UserRepository repo(":memory:");
    repo.createUser("Ruth", "hash1");
    bool second = repo.createUser("Ruth", "hash2");
    REQUIRE_FALSE(second);
}

TEST_CASE("UserRepository: updateElo changes the stored elo", "[UserRepository]") {
    UserRepository repo(":memory:");
    repo.createUser("Ruth", "somehash");
    auto user = repo.findByUsername("Ruth");

    repo.updateElo(user->id, 1350);

    auto updated = repo.findByUsername("Ruth");
    REQUIRE(updated->elo == 1350);
}
