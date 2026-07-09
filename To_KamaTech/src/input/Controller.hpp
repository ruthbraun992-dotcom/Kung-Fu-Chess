// Controller.hpp
#pragma once
#include "engine/GameEngine.hpp"
#include "BoardMapper.hpp"
#include <optional>

class Controller {
public:
    explicit Controller(GameEngine& engine) : engine_(engine) {}
    void click(int x, int y);

private:
    GameEngine& engine_;
    std::optional<Position> selected_;
};