#include <catch2/catch_test_macros.hpp>
#include "events/EventBus.hpp"
#include "events/Events.hpp"

TEST_CASE("EventBus: subscriber receives published event", "[EventBus]") {
    EventBus bus;
    bool called = false;
    Piece::Color receivedColor{};
    int receivedScore = 0;

    bus.subscribe<ScoreUpdatedEvent>([&](const ScoreUpdatedEvent& e) {
        called = true;
        receivedColor = e.color;
        receivedScore = e.newScore;
    });

    bus.publish(ScoreUpdatedEvent{Piece::Color::WHITE, 5});

    REQUIRE(called);
    REQUIRE(receivedColor == Piece::Color::WHITE);
    REQUIRE(receivedScore == 5);
}

TEST_CASE("EventBus: publish with no subscribers does not crash", "[EventBus]") {
    EventBus bus;
    REQUIRE_NOTHROW(bus.publish(SoundEvent{SoundType::Move}));
}

TEST_CASE("EventBus: multiple subscribers to same event all get called", "[EventBus]") {
    EventBus bus;
    int callCount = 0;

    bus.subscribe<SoundEvent>([&](const SoundEvent&) { callCount++; });
    bus.subscribe<SoundEvent>([&](const SoundEvent&) { callCount++; });
    bus.subscribe<SoundEvent>([&](const SoundEvent&) { callCount++; });

    bus.publish(SoundEvent{SoundType::Capture});

    REQUIRE(callCount == 3);
}

TEST_CASE("EventBus: subscriber to one event type not called for a different type", "[EventBus]") {
    EventBus bus;
    bool soundCalled = false;
    bool moveCalled = false;

    bus.subscribe<SoundEvent>([&](const SoundEvent&) { soundCalled = true; });
    bus.subscribe<MoveLoggedEvent>([&](const MoveLoggedEvent&) { moveCalled = true; });

    bus.publish(SoundEvent{SoundType::GameStart});

    REQUIRE(soundCalled);
    REQUIRE_FALSE(moveCalled);
}

TEST_CASE("EventBus: publishing multiple times calls subscriber each time", "[EventBus]") {
    EventBus bus;
    int callCount = 0;

    bus.subscribe<GameStartedEvent>([&](const GameStartedEvent&) { callCount++; });

    bus.publish(GameStartedEvent{});
    bus.publish(GameStartedEvent{});
    bus.publish(GameStartedEvent{});

    REQUIRE(callCount == 3);
}

TEST_CASE("EventBus: event data is passed correctly with Position and Piece::Type", "[EventBus]") {
    EventBus bus;
    Position receivedPos{-1, -1};
    Piece::Type receivedType{};

    bus.subscribe<PieceCapturedEvent>([&](const PieceCapturedEvent& e) {
        receivedPos = e.atSquare;
        receivedType = e.pieceType;
    });

    Position pos{3, 4};
    bus.publish(PieceCapturedEvent{Piece::Color::BLACK, Piece::Type::QUEEN, pos});

    REQUIRE(receivedPos == pos);
    REQUIRE(receivedType == Piece::Type::QUEEN);
}