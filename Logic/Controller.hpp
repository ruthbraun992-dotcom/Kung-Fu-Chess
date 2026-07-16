#pragma once
#include "GameEngine.hpp"
#include "BoardMapper.hpp"
#include <optional>

class Controller {
public:
    explicit Controller(GameEngine& engine) : engine_(engine) {}
    void click(const Position& cell);
    void jump(const Position& cell);

    std::optional<Position> getSelected() const { return selected_; }

private:
    GameEngine& engine_;
    std::optional<Position> selected_;
};