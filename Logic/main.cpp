// #include "GameEngine.hpp"
// #include "Controller.hpp"
// #include "BoardParser.hpp"
// #include "ParseError.hpp"
// #include <iostream>
// #include <sstream>
// #include <string>
// #include <vector>
// #include <memory>

// int main() {
//     std::vector<std::string> boardLines;
//     std::string line;

//     while (std::getline(std::cin, line)) {
//         if (line.find("Board:") != std::string::npos)
//             break;
//     }

//     while (std::getline(std::cin, line)) {
//         if (line.find("Commands:") != std::string::npos)
//             break;
//         if (!line.empty())
//             boardLines.push_back(line);
//     }
//     std::unique_ptr<GameEngine> enginePtr;
//     try {
//         Board board = BoardParser::parse(boardLines);
//         enginePtr = std::make_unique<GameEngine>(std::move(board));
//     } catch (const ParseError& e) {
//         std::cout << "ERROR " << e.what() << "\n";
//         return 0;
//     }

//     Controller controller(*enginePtr);

//     while (std::getline(std::cin, line)) {
//         std::istringstream iss(line);
//         std::string cmd;
//         if (!(iss >> cmd)) continue;

//         if (cmd == "click") {
//             int x, y; iss >> x >> y;
//             controller.click(x, y);
//         } else if (cmd == "jump") {
//             int x, y; iss >> x >> y;
//             controller.jump(x, y);
//         } else if (cmd == "wait") {
//             long ms; iss >> ms;
//             enginePtr->wait(ms);
//         } else if (cmd == "print") {
//             enginePtr->printBoard(std::cout);
//         }
//     }
//     return 0;
// }