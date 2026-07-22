#pragma once
#include "GameEngine.hpp"
#include "rules/BoardMapper.hpp"
#include <optional>

class Controller {
public:
    explicit Controller(GameEngine& engine) : engine_(engine) {}
    void click(const Position& cell);
    void jump(const Position& cell);
    bool Controller::isGameOver() const {return engine_.isGameOver();}
    std::optional<Position> getSelected() const { return selected_; }
    void Controller::clickOutside(){selected_.reset();}
private:
    GameEngine& engine_;
    std::optional<Position> selected_;
};