#pragma once

#include "realtime/RealTimeArbiter.hpp"
#include "animation/AnimationConfigLoader.hpp"
#include "model/Board.hpp"
#include "RenderPosition.hpp"
#include "PieceState.hpp"
#include "stats/GameStats.hpp"
#include "events/EventBus.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
class GameEngine {
public:
    GameEngine(Board board, AnimationConfigLoader configs, EventBus& bus)
    : board_(std::move(board)),
      configs_(std::move(configs)),
      arbiter_(configs_),
      bus_(bus)
{}
    bool requestMove(int fromRow, int fromCol, int toRow, int toCol);
    void applyServerMove(int fromRow,int fromCol,int toRow, int toCol);
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
    std::optional<long> stateStartTimeOf(const Position& from) const; 
    std::optional<long> stateDurationOf(const Position& from) const;
    void startMotion(const Motion& motion);
private:
    Board board_;
    AnimationConfigLoader configs_;    RealTimeArbiter arbiter_;
    bool gameOver_ = false;
    GameStats stats_;
    EventBus& bus_;
};