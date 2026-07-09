#include "BoardParser.hpp"
#include "ParseError.hpp"
#include <sstream>

Board BoardParser::parse(const std::vector<std::string>& boardLines) {
    std::vector<std::vector<std::string>> tokenRows;
    tokenRows.reserve(boardLines.size());

    for (const auto& line : boardLines) {
        std::vector<std::string> tokens = splitTokens(line);
        if (tokens.empty()) continue; // skip blank lines defensively
        validateTokens(tokens);
        tokenRows.push_back(tokens);
    }

    if (tokenRows.empty()) {
        throw ParseError("ROW_WIDTH_MISMATCH");
    }

    size_t width = tokenRows[0].size();
    for (const auto& row : tokenRows) {
        if (row.size() != width) {
            throw ParseError("ROW_WIDTH_MISMATCH");
        }
    }

    Board board(static_cast<int>(tokenRows.size()), static_cast<int>(width));
    for (size_t r = 0; r < tokenRows.size(); ++r) {
        for (size_t c = 0; c < width; ++c) {
            const std::string& token = tokenRows[r][c];
            if (token == ".") {
                board.setCell(static_cast<int>(r), static_cast<int>(c), std::nullopt);
            } else {
                board.setCell(static_cast<int>(r), static_cast<int>(c), Piece::fromToken(token));
            }
        }
    }
    return board;
}

std::vector<std::string> BoardParser::splitTokens(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

void BoardParser::validateTokens(const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        if (token == ".") continue;
        if (!Piece::fromToken(token).has_value()) {
            throw ParseError("UNKNOWN_TOKEN");
        }
    }
}