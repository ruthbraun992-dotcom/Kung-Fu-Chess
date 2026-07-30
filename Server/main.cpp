// // Server/main.cpp
 #include "WebSocketServer.hpp"
 #include "GameManager.hpp"
 #include "UserRepository.hpp"
#include "Logger.hpp"
// int main()
// {
//     UserRepository users("chess_users.db");
//     GameManager games;

//     WebSocketServer server(games, users, 9002);
//     server.run();

//     return 0;
// }


#include <filesystem>
#include <iostream>

int main()
{
    std::cout << "Current path: "
              << std::filesystem::current_path() << '\n';

    std::cout << "DB path: "
              << std::filesystem::absolute("chess_users.db") << '\n';

              Logger::instance().openFile("server.log");

    UserRepository users("chess_users.db");
    GameManager games;

    std::thread tickThread([&games]() {
        using namespace std::chrono;
        while (true) {
            std::this_thread::sleep_for(milliseconds(50));
            games.update(50);
        }
    });
    tickThread.detach();

    WebSocketServer server(games, users, 9002);
    server.run();
   
}