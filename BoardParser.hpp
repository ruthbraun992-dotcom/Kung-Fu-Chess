#ifndef BOARD_PARSER_H
#define BOARD_PARSER_H

#include <vector>
#include <string>
#include "Board.hpp"

// Turns the raw "Board:" section lines into a validated Board.
// Throws ParseError (see ParseError.h) on invalid input.
class BoardParser {
public:
    static Board parse(const std::vector<std::string>& boardLines);

private:
    static std::vector<std::string> splitTokens(const std::string& line);
    static void validateTokens(const std::vector<std::string>& tokens);
};

#endif // BOARD_PARSER_H