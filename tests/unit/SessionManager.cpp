#include <catch2/catch_test_macros.hpp>
#include "../../Server/SessionManager.hpp"
#include <memory>

// עוזר לטסטים: יוצר connection_hdl "מזויף" אבל תקין להשוואה
websocketpp::connection_hdl makeFakeHandle() {
    // כל קריאה יוצרת shared_ptr חדש -> weak_ptr שונה -> hdl "שונה" לצורך זיהוי
    auto obj = std::make_shared<int>(0);
    return websocketpp::connection_hdl(obj);
}

TEST_CASE("SessionManager: registerConnection returns increasing ids", "[SessionManager]") {
    SessionManager sm;
    int id1 = sm.registerConnection(makeFakeHandle());
    int id2 = sm.registerConnection(makeFakeHandle());

    REQUIRE(id1 != id2);
}

TEST_CASE("SessionManager: setUsername and getUsername round-trip", "[SessionManager]") {
    SessionManager sm;
    int id = sm.registerConnection(makeFakeHandle());

    sm.setUsername(id, "Ruth");

    auto name = sm.getUsername(id);
    REQUIRE(name.has_value());
    REQUIRE(*name == "Ruth");
}

TEST_CASE("SessionManager: getUsername returns nullopt before login", "[SessionManager]") {
    SessionManager sm;
    int id = sm.registerConnection(makeFakeHandle());

    auto name = sm.getUsername(id);
    REQUIRE_FALSE(name.has_value());
}

TEST_CASE("SessionManager: removeConnection clears username and handle", "[SessionManager]") {
    SessionManager sm;
    int id = sm.registerConnection(makeFakeHandle());
    sm.setUsername(id, "Ruth");

    sm.removeConnection(id);

    REQUIRE_FALSE(sm.getUsername(id).has_value());
}

TEST_CASE("SessionManager: findSessionByHandle finds the correct session", "[SessionManager]") {
    SessionManager sm;
    auto hdl1 = makeFakeHandle();
    auto hdl2 = makeFakeHandle();

    int id1 = sm.registerConnection(hdl1);
    int id2 = sm.registerConnection(hdl2);

    auto found1 = sm.findSessionByHandle(hdl1);
    auto found2 = sm.findSessionByHandle(hdl2);

    REQUIRE(found1.has_value());
    REQUIRE(*found1 == id1);
    REQUIRE(found2.has_value());
    REQUIRE(*found2 == id2);
}

TEST_CASE("SessionManager: findSessionByHandle returns nullopt for unknown handle", "[SessionManager]") {
    SessionManager sm;
    sm.registerConnection(makeFakeHandle());

    auto unknownHdl = makeFakeHandle(); // אף פעם לא נרשם
    auto found = sm.findSessionByHandle(unknownHdl);

    REQUIRE_FALSE(found.has_value());
}

TEST_CASE("SessionManager: allSessionIds reflects registered and removed sessions", "[SessionManager]") {
    SessionManager sm;
    int id1 = sm.registerConnection(makeFakeHandle());
    int id2 = sm.registerConnection(makeFakeHandle());

    auto ids = sm.allSessionIds();
    REQUIRE(ids.size() == 2);

    sm.removeConnection(id1);
    ids = sm.allSessionIds();
    REQUIRE(ids.size() == 1);
    REQUIRE(ids[0] == id2);
}