#ifndef IO_H
#define IO_H

#include <vector>
#include <string>
#include <istream>
#include <ostream>
#include "Game.hpp"

// Removes leading/trailing whitespace. Shared by InputReader and
// CommandProcessor so the trimming rule lives in exactly one place.
std::string trimWhitespace(const std::string& s);

// Splits raw stdin into the "Board:" and "Commands:" sections.
class InputReader {
public:
    struct Sections {
        std::vector<std::string> boardLines;
        std::vector<std::string> commandLines;
    };

    static Sections read(std::istream& in);
};

// Parses and executes the "Commands:" section against a Game:
// "click X Y", "wait MS", "print board".
class CommandProcessor {
public:
    static void run(Game& game,
                     const std::vector<std::string>& commandLines,
                     std::ostream& out);
};

#endif // IO_H