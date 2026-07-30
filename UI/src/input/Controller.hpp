#pragma once

#include "Position.hpp"
#include <optional>

class GameClient;

class Controller
{
public:
    Controller(GameClient& client);

    void click(const Position& cell);
    void clickOutside();

    std::optional<Position> getSelected() const;

private:
    GameClient& client_;
    std::optional<Position> selected_;
};