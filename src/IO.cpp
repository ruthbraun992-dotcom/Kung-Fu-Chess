#include "IO.hpp"
#include <sstream>

namespace {
    // Section markers and command keywords, named in one place instead
    // of scattered as bare string literals through the parsing logic.
    constexpr const char* SECTION_BOARD    = "Board:";
    constexpr const char* SECTION_COMMANDS = "Commands:";
    constexpr const char* CMD_CLICK        = "click";
    constexpr const char* CMD_WAIT         = "wait";
    constexpr const char* CMD_PRINT        = "print";
    constexpr const char* CMD_PRINT_TARGET = "board";
}

std::string trimWhitespace(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

InputReader::Sections InputReader::read(std::istream& in) {
    Sections sections;
    enum class State { NONE, BOARD, COMMANDS };
    State state = State::NONE;

    std::string line;
    while (std::getline(in, line)) {
        std::string trimmed = trimWhitespace(line);

        if (trimmed == SECTION_BOARD) {
            state = State::BOARD;
            continue;
        }
        if (trimmed == SECTION_COMMANDS) {
            state = State::COMMANDS;
            continue;
        }

        if (state == State::BOARD) {
            sections.boardLines.push_back(line);
        } else if (state == State::COMMANDS) {
            sections.commandLines.push_back(line);
        }
    }
    return sections;
}

void CommandProcessor::run(Game& game,
                            const std::vector<std::string>& commandLines,
                            std::ostream& out) {
    for (const auto& raw : commandLines) {
        std::string cmd = trimWhitespace(raw);
        if (cmd.empty()) continue;

        std::istringstream iss(cmd);
        std::string keyword;
        iss >> keyword;

        if (keyword == CMD_CLICK) {
            int x, y;
            if (iss >> x >> y) {
                game.handleClick(x, y);
            }
        } else if (keyword == CMD_WAIT) {
            long ms;
            if (iss >> ms) {
                game.handleWait(ms);
            }
        } else if (keyword == CMD_PRINT) {
            std::string second;
            if (iss >> second && second == CMD_PRINT_TARGET) {
                game.printBoard(out);
            }
        }
        // Unknown/unsupported commands are silently ignored for now.
    }
}