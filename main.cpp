#include <iostream>
#include "IO.hpp"
#include "BoardParser.hpp"
#include "ParseError.hpp"

int main() {
    std::ios::sync_with_stdio(false);

    InputReader::Sections sections = InputReader::read(std::cin);

    try {
        Board board = BoardParser::parse(sections.boardLines);
        Game game(std::move(board));
        CommandProcessor::run(game, sections.commandLines, std::cout);
    } catch (const ParseError& e) {
        std::cout << "ERROR " << e.code() << "\n";
        return 0;
    }

    return 0;
}