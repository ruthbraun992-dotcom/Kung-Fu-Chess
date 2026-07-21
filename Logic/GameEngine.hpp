#pragma once
#include "RealTimeArbiter.hpp"
#include "AnimationConfigLoader.hpp"
#include <ostream>
#include "Board.hpp"
#include "RenderPosition.hpp"
#include "PieceState.hpp"
#include "GameStats.hpp"
class GameEngine {
public:
    GameEngine(Board board, const AnimationConfigLoader& configs)
        : board_(std::move(board)), configs_(configs), arbiter_(configs) {}

    bool requestMove(int fromRow, int fromCol, int toRow, int toCol);
    bool requestJump(int row, int col);
    void printBoard(std::ostream& out) const { board_.Printer(out); }
    const Board& board() const { return board_; }
    bool isGameOver() const { return gameOver_; }
    const Board& getBoard() const { return board_; }
    void update(long ms);
    std::optional<RenderPosition> currentPositionOf(const Position& from) const;
    long currentTime() const;
    std::optional<PieceState> currentStateOf(const Position& from) const;
    const GameStats& stats() const { return stats_; }
    std::optional<long> stateStartTimeOf(const Position& from) const;   // חדש
    std::optional<long> stateDurationOf(const Position& from) const;

private:
    Board board_;
    const AnimationConfigLoader& configs_;
    RealTimeArbiter arbiter_;
    bool gameOver_ = false;
    GameStats stats_;
};