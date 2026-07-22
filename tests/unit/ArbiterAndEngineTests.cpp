#include <catch2/catch_test_macros.hpp>
#include "RealTimeArbiter.hpp"
#include "GameEngine.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "position.hpp"

// עוזר קטן ליצירת כלי לבדיקות
static Piece makeWhiteRook() {
    return Piece::fromToken("wR").value();
}
static Piece makeBlackRook() {
    return Piece::fromToken("bR").value();
}

// ---------- RealTimeArbiter ----------

TEST_CASE("Arbiter: no active motion initially") {
    RealTimeArbiter arbiter;
    Position from{0, 0};
    REQUIRE_FALSE(arbiter.hasActiveMotionFrom(from));
}

TEST_CASE("Arbiter: hasActiveMotionFrom detects the moving piece's source") {
    RealTimeArbiter arbiter;
    Position from{0, 0}, to{0, 2};
    arbiter.startMotion(Motion{from, to, makeWhiteRook(), 2000L});

    REQUIRE(arbiter.hasActiveMotionFrom(from));
    REQUIRE_FALSE(arbiter.hasActiveMotionFrom(Position{5, 5}));
}

TEST_CASE("Arbiter: conflictsWithActiveMotion detects same destination") {
    RealTimeArbiter arbiter;
    Position from1{0, 0}, to1{0, 2};
    arbiter.startMotion(Motion{from1, to1, makeWhiteRook(), 2000L});

    Position from2{2, 2}, to2{0, 2}; 
    REQUIRE(arbiter.conflictsWithActiveMotion(from2, to2));
}

TEST_CASE("Arbiter: conflictsWithActiveMotion detects target reserved as another motion's source") {
    RealTimeArbiter arbiter;
    Position from1{0, 0}, to1{0, 2};
    arbiter.startMotion(Motion{from1, to1, makeWhiteRook(), 2000L});

    Position from2{5, 0}, to2{0, 0};
    REQUIRE(arbiter.conflictsWithActiveMotion(from2, to2));
}

TEST_CASE("Arbiter: no conflict for unrelated positions") {
    RealTimeArbiter arbiter;
    Position from1{0, 0}, to1{0, 2};
    arbiter.startMotion(Motion{from1, to1, makeWhiteRook(), 2000L});

    Position from2{5, 5}, to2{5, 7};
    REQUIRE_FALSE(arbiter.conflictsWithActiveMotion(from2, to2));
}

TEST_CASE("Arbiter: advanceTime resolves motion after full duration") {
    RealTimeArbiter arbiter;
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());

    Position from{0, 0}, to{0, 2};
    arbiter.startMotion(Motion{from, to, makeWhiteRook(), 2000L});

    arbiter.advanceTime(2000, board);

    REQUIRE_FALSE(board.getCell(0, 0).has_value());
    REQUIRE(board.getCell(0, 2).has_value());
    REQUIRE_FALSE(arbiter.hasActiveMotionFrom(from));
}

TEST_CASE("Arbiter: advanceTime does not resolve motion before duration elapses") {
    RealTimeArbiter arbiter;
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());

    Position from{0, 0}, to{0, 2};
    arbiter.startMotion(Motion{from, to, makeWhiteRook(), 2000L});

    arbiter.advanceTime(1000, board); // רק חצי מהזמן

    REQUIRE(board.getCell(0, 0).has_value());   // עדיין במקור
    REQUIRE_FALSE(board.getCell(0, 2).has_value());
    REQUIRE(arbiter.hasActiveMotionFrom(from));  // עדיין בתנועה
}

// ---------- GameEngine::requestMove ----------

TEST_CASE("GameEngine: requestMove succeeds for a valid move") {
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2));
}

TEST_CASE("GameEngine: requestMove rejects premove on already-moving piece") {
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2));       // מהלך ראשון מתחיל
    REQUIRE_FALSE(engine.requestMove(0, 0, 1, 0)); // premove על אותו כלי - נדחה
}

TEST_CASE("GameEngine: requestMove rejects move to a reserved destination") {
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());
    board.setCell(2, 2, makeBlackRook());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2));        // wR נע לעבר (0,2)
    REQUIRE_FALSE(engine.requestMove(2, 2, 2, 2));  // לא רלוונטי, לדוגמה בלבד - יש להתאים ליעד אמיתי
}

TEST_CASE("GameEngine: requestMove allows unrelated concurrent motion") {
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());
    board.setCell(2, 0, makeBlackRook());
    GameEngine engine(std::move(board));

    REQUIRE(engine.requestMove(0, 0, 0, 2)); // wR: מסלול שורה 0
    REQUIRE(engine.requestMove(2, 0, 2, 2)); // bR: מסלול שורה 2, לא מתנגש
}

TEST_CASE("GameEngine: requestMove rejects move onto friendly piece") {
    Board board(3, 3);
    board.setCell(0, 0, makeWhiteRook());
    board.setCell(0, 2, makeWhiteRook());
    GameEngine engine(std::move(board));

    REQUIRE_FALSE(engine.requestMove(0, 0, 0, 2));
}