#include "GameEngine.hpp"
#include "Controller.hpp"
#include "BoardParser.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// מסיר רווחים מובילים/סוגרים משורה
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

int main() {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(std::cin, line)) {
        lines.push_back(line);
    }

    // שלב 1: איתור "Board:" ואיסוף שורות הלוח עד "Commands:"
    std::vector<std::string> boardLines;
    size_t i = 0;

    while (i < lines.size() && trim(lines[i]) != "Board:") {
        ++i;
    }
    ++i; // לדלג על שורת "Board:" עצמה

    while (i < lines.size() && trim(lines[i]) != "Commands:") {
        std::string t = trim(lines[i]);
        if (!t.empty()) {
            boardLines.push_back(t);
        }
        ++i;
    }
    ++i; // לדלג על שורת "Commands:" עצמה

    // שלב 2: פרסור הלוח והקמת המנוע
    Board board = BoardParser::parse(boardLines);
    GameEngine engine(std::move(board));
    Controller controller(engine);

    // שלב 3: עיבוד הפקודות אחת-אחת
    for (; i < lines.size(); ++i) {
        std::string t = trim(lines[i]);
        if (t.empty()) continue;

        std::istringstream iss(t);
        std::string cmd;
        iss >> cmd;

        if (cmd == "click") {
            int x, y;
            iss >> x >> y;
            controller.click(x, y);
        } else if (cmd == "wait") {
            long ms;
            iss >> ms;
            engine.wait(ms);
        } 
         else if (cmd == "jump") {
        int x, y; iss >> x >> y;
        controller.jump(x, y);
        }
        else if (cmd == "print") {
            std::string what;
            iss >> what; // "board"
            engine.printBoard(std::cout);
        }
    }
    return 0;
}