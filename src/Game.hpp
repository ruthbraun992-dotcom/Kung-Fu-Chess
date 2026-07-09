#ifndef GAME_H
#define GAME_H

#include <optional>
#include <ostream>
#include "Board.hpp"

// Tracks the elapsed in-game time, advanced by "wait" commands.
// Currently informational only (no move durations yet), but this
// is the seam future iterations will hook cooldowns/travel-time into.
class GameClock {
public:
    GameClock() : currentMs_(0) {}

    void advance(long ms) { currentMs_ += ms; }
    long now() const { return currentMs_; }

private:
    long currentMs_;
};

// Owns the board and the current click-selection state.
// Interprets "click"/"wait" at the game-rules level; ClickTranslator
// (in Board.h) only handles the pixel -> cell geometry.
class Game {
public:
    explicit Game(Board board);

    void handleClick(int x, int y);
    void handleWait(long ms);
    void printBoard(std::ostream& out) const;

private:
    Board board_;
    GameClock clock_;
    std::optional<Position> selected_;
    struct PendingMove {
    Position from;
    Position to;
    Piece piece;
    long arrivalTime;
};

    std::optional<PendingMove> pendingMove_;

    static bool sameColor(const Piece& a, const Piece& b);
};

#endif // GAME_H