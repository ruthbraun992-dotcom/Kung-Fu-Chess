#pragma once
#include "RealTimeArbiter.hpp"
#include <ostream>
#include "Board.hpp"

class GameEngine {
public:
    explicit GameEngine(Board board) : board_(std::move(board)) {}

    bool requestMove(int fromRow, int fromCol, int toRow, int toCol);
    void wait(long ms);
    void printBoard(std::ostream& out) const { board_.Printer(out); }
    const Board& board() const { return board_; }
    bool isGameOver() const { return gameOver_; }

private:
    Board board_;
    RealTimeArbiter arbiter_;
    bool gameOver_ = false;
};