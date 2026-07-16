#pragma once
#include "RealTimeArbiter.hpp"
#include <ostream>
#include "Board.hpp"

class GameEngine {
public:
    explicit GameEngine(Board board) : board_(std::move(board)) {}

    bool requestMove(int fromRow, int fromCol, int toRow, int toCol);
    bool requestJump(int row, int col);
    void printBoard(std::ostream& out) const { board_.Printer(out); }
    const Board& board() const { return board_; }
    bool isGameOver() const { return gameOver_; }
    const Board& getBoard() const
    {
        return board_;
    }
    void update(long ms);
private:
    Board board_;
    RealTimeArbiter arbiter_;
    bool gameOver_ = false;
};