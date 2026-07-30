#include "Controller.hpp"
#include <iostream>
#include "../networking/GameClient.hpp"


Controller::Controller(GameClient& client)
    : client_(client)
{
}


void Controller::click(const Position& cell)
{
    if (!selected_.has_value())
    {
        selected_ = cell;

        std::cout 
            << "Selected: "
            << cell.row << ","
            << cell.col
            << std::endl;

        return;
    }


    Position from = *selected_;


    if (from.row == cell.row &&
        from.col == cell.col)
    {
        selected_.reset();
        return;
    }


    std::cout 
        << "Sending move: ("
        << from.row << ","
        << from.col
        << ") -> ("
        << cell.row << ","
        << cell.col
        << ")"
        << std::endl;


    client_.sendMove(
        from.row,
        from.col,
        cell.row,
        cell.col
    );


    selected_.reset();
}


void Controller::clickOutside()
{
    selected_.reset();
}


std::optional<Position> Controller::getSelected() const
{
    return selected_;
}