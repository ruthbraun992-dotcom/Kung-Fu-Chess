#pragma once
#include "GameEngine.hpp"
#include "Position.hpp"
#include <optional>
#include <memory>

class GameClient;

class Controller {
public:
    Controller(GameEngine& engine, GameClient* gameClient = nullptr);
    void click(const Position& cell);
    void jump(const Position& cell);
    void clickOutside();
    
    bool isGameOver() const { return engine_.isGameOver(); }
    std::optional<Position> getSelected() const { return selected_; }
    
    void setGameClient(GameClient* gameClient) {
        gameClient_ = gameClient;
    }
    
private:
    GameEngine& engine_;
    GameClient* gameClient_;
    std::optional<Position> selected_;
};