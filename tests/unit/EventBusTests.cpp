// tests/unit/EventBusTests.cpp
#include <catch2/catch_test_macros.hpp>
#include "EventBus.hpp"

// אירועי-דמה לבדיקה בלבד, לא קשורים ל-Events.hpp האמיתי
struct TestEventA { int value; };
struct TestEventB { std::string text; };

TEST_CASE("EventBus: subscriber receives published event", "[EventBus]") {
    EventBus bus;
    bool called = false;
    int received = 0;

    bus.subscribe<TestEventA>([&](const TestEventA& e) {
        called = true;
        received = e.value;
    });

    bus.publish(TestEventA{42});

    REQUIRE(called);
    REQUIRE(received == 42);
}

TEST_CASE("EventBus: publish with no subscribers does nothing (no crash)", "[EventBus]") {
    EventBus bus;
    REQUIRE_NOTHROW(bus.publish(TestEventA{1}));
}

TEST_CASE("EventBus: multiple subscribers to same event all get called", "[EventBus]") {
    EventBus bus;
    int count = 0;

    bus.subscribe<TestEventA>([&](const TestEventA&) { count++; });
    bus.subscribe<TestEventA>([&](const TestEventA&) { count++; });
    bus.subscribe<TestEventA>([&](const TestEventA&) { count++; });

    bus.publish(TestEventA{0});

    REQUIRE(count == 3);
}

TEST_CASE("EventBus: different event types don't cross-trigger", "[EventBus]") {
    EventBus bus;
    bool aCalled = false;
    bool bCalled = false;

    bus.subscribe<TestEventA>([&](const TestEventA&) { aCalled = true; });
    bus.subscribe<TestEventB>([&](const TestEventB&) { bCalled = true; });

    bus.publish(TestEventA{1});

    REQUIRE(aCalled);
    REQUIRE_FALSE(bCalled);   // חשוב: B לא אמור להיקרא כי פרסמנו רק A
}

TEST_CASE("EventBus: event data is passed correctly by value/const-ref", "[EventBus]") {
    EventBus bus;
    std::string captured;

    bus.subscribe<TestEventB>([&](const TestEventB& e) {
        captured = e.text;
    });

    bus.publish(TestEventB{"hello"});

    REQUIRE(captured == "hello");
}

TEST_CASE("EventBus: publish called multiple times triggers handler each time", "[EventBus]") {
    EventBus bus;
    int callCount = 0;

    bus.subscribe<TestEventA>([&](const TestEventA&) { callCount++; });

    bus.publish(TestEventA{1});
    bus.publish(TestEventA{2});
    bus.publish(TestEventA{3});

    REQUIRE(callCount == 3);
}