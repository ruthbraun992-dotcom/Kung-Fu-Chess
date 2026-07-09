#include "engine/GameEngine.hpp"
#include "input/Controller.hpp"
#include "io/BoardParser.hpp"
#include <iostream>
#include "engine/GameEngine.hpp"

int main() {
    Board board(8, 8); // או תוצאה של BoardParser::parse(...)
    GameEngine engine(std::move(board));
    Controller controller(engine);

    // ... לולאת אפליקציה / DSL runner משתמשים ב-controller.click ו-engine.wait/printBoard
    return 0;
}