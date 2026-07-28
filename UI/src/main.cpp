#include "sprites/img.hpp"
#include "input/MouseHandler.hpp"
#include "input/ClickTranslator.hpp"
#include "sprites/SpriteManager.hpp"
#include "rendering/BoardRenderer.hpp"
#include "rendering/BoardSetup.hpp"
#include "model/Board.hpp"
#include "position.hpp"
#include "engine/GameEngine.hpp"
#include "engine/Controller.hpp"
#include "rendering/MovesLogRenderer.hpp"
#include "networking/GameClient.hpp"

#include <filesystem>
#include <optional>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

namespace
{
std::filesystem::path resolveAssetPath(const std::filesystem::path& relativePath)
{
    std::filesystem::path base = std::filesystem::path(__FILE__).parent_path();
    if (!base.is_absolute())
    {
        base = std::filesystem::absolute(std::filesystem::current_path() / base);
    }

    std::vector<std::filesystem::path> candidates;
    std::filesystem::path current = base;

    while (true)
    {
        candidates.push_back(current / relativePath);
        candidates.push_back(current / "src" / relativePath);
        candidates.push_back(current / "cpp" / relativePath);

        const std::filesystem::path parent = current.parent_path();
        if (parent == current) break;
        current = parent;
    }

    candidates.push_back(std::filesystem::current_path() / relativePath);

    for (const auto& candidate : candidates)
    {
        const std::filesystem::path normalized = candidate.lexically_normal();
        if (std::filesystem::exists(normalized))
            return normalized;
    }

    return candidates.front().lexically_normal();
}
}

int main(int argc, char* argv[])
{
    try
    {
        // ========== SECTION 1: USERNAME FROM COMMAND LINE ==========
        std::string username = "player1";
        std::string password = "pass123";
        
        if (argc > 1) username = argv[1];
        if (argc > 2) password = argv[2];
        
        std::cout << "\n================================" << std::endl;
        std::cout << "👤 USERNAME: " << username << std::endl;
        std::cout << "================================\n" << std::endl;

        // ========== SECTION 2: LOAD GRAPHICS ==========
        const std::filesystem::path spriteDir = resolveAssetPath("../pieces6");
        const std::filesystem::path boardImagePath = resolveAssetPath("../pieces6/board.png");
        const std::filesystem::path gameOverPath = resolveAssetPath("../GameOver.png");
        
        Img boardImage;
        Img gameOverImage;
        
        gameOverImage.read(gameOverPath.string(), {800, 800}, false);
        boardImage.read(boardImagePath.string(), {650, 650}, false);
        
        constexpr int kBoardSize = 960;
        const int cellSize = 63;
        const int offsetX = 90;
        const int offsetY = 90;

        const cv::Mat& img = boardImage.get_mat();

        // ========== SECTION 3: SETUP GAME ENGINE (LOCAL) ==========
        Board board(8, 8);
        setupStartingPosition(board);
        EventBus bus;
        AnimationConfigLoader animConfigs(spriteDir.string());
        GameEngine engine(std::move(board), animConfigs, bus);

        // ========== SECTION 4: CREATE GAME CLIENT ==========
        GameClient gameClient;
        
        // ========== SECTION 5: CREATE CONTROLLER ==========
        Controller controller(engine, &gameClient);

        // ========== SECTION 6: SETUP GRAPHICS RENDERING ==========
        SpriteManager sprites(spriteDir.string());
        BoardRenderer renderer(8, 8, cellSize, sprites, offsetX, offsetY, engine);
        MovesLogRenderer logRenderer(250, 800, 8, 8);

        cv::Mat canvas;
        const int logColWidth = 250;
        ClickTranslator translator(8, 8, cellSize, offsetX + logColWidth, offsetY);
        MouseHandler mouse(translator);

        // ========== SECTION 7: SETUP MESSAGE HANDLER ==========
        // ✅ NEW CODE STARTS HERE
        gameClient.onMessage = [&](const json& msg) {
            std::string type = msg.value("type", "");
            
            if (type == "moveLogged") {
                std::string moveText = msg.value("move", "");
                std::string color = msg.value("color", "");
                std::cout << "✅ Move logged: " << color << " - " << moveText << std::endl;
            }
            else if (type == "sound") {
                std::cout << "🔊 Sound event" << std::endl;
            }
            else if (type == "pieceCaptured") {
                std::cout << "💣 Piece captured!" << std::endl;
            }
            else if (type == "gameStarting") {
                std::string color = msg.value("color", "");
                std::string opponent = msg.value("opponent", "");
                std::cout << "\n🎮🎮🎮 GAME STARTED! 🎮🎮🎮" << std::endl;
                std::cout << "You are: " << color << std::endl;
                std::cout << "Opponent: " << opponent << std::endl;
                std::cout << "🎮🎮🎮🎮🎮🎮🎮🎮🎮🎮🎮🎮\n" << std::endl;
            }
            else if (type == "opponentDisconnected") {
                int secondsLeft = msg.value("secondsLeft", 0);
                std::cout << "❌ Opponent disconnected! " << secondsLeft << "s remaining" << std::endl;
            }
            else if (type == "gameOver") {
                std::string winner = msg.value("winner", "");
                std::string reason = msg.value("reason", "");
                std::cout << "🏁 GAME OVER! Winner: " << winner << std::endl;
            }
        };

        // ✅ NEW: Board state handler
        gameClient.onBoardStateUpdate = [&](const json& boardState) {
            std::cout << "📊 Board state updated from server" << std::endl;
            
            // Update engine's board
            engine.updateBoardFromServer(boardState);
            
            std::cout << "✅ Board synced" << std::endl;
        };
        // ✅ NEW CODE ENDS HERE

        // ========== SECTION 8: CONNECT TO SERVER ==========
        std::cout << "🔗 Connecting to server..." << std::endl;
        if (!gameClient.connect("ws://localhost:9002")) {
            std::cerr << "❌ Failed to connect to server." << std::endl;
            return 1;
        }
        
        std::cout << "✅ Connected to server!" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // ========== SECTION 9: REGISTER ==========
        std::cout << "📝 Registering..." << std::endl;
        gameClient.registerUser(username, password);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // ========== SECTION 10: LOGIN ==========
        std::cout << "🔐 Logging in..." << std::endl;
        gameClient.login(username, password);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // ========== SECTION 11: CREATE OR JOIN ROOM ==========
        if (username == "alice") {
            std::cout << "👸 Creating room R1..." << std::endl;
            gameClient.createRoom();
        } else {
            std::cout << "👑 Joining room R1..." << std::endl;
            gameClient.joinRoom("R1");
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // ========== SECTION 12: REDRAW FUNCTION ==========
        auto redraw = [&]()
        {
            if (controller.isGameOver())
            {
                canvas = gameOverImage.get_mat().clone();
            }
            else
            {
                renderer.setSelectedCell(controller.getSelected());
                renderer.draw(img, engine.board(), canvas);
            }

            cv::Mat blackPart = logRenderer.renderColumn(engine.stats(), Piece::Color::BLACK);
            cv::Mat whitePart = logRenderer.renderColumn(engine.stats(), Piece::Color::WHITE);

            auto matchHeight = [&](cv::Mat& m) {
                if (m.rows != canvas.rows)
                    cv::resize(m, m, cv::Size(m.cols, canvas.rows));
            };
            matchHeight(blackPart);
            matchHeight(whitePart);

            auto to4ch = [&](cv::Mat& m) {
                if (canvas.channels() == 4 && m.channels() == 3)
                {
                    cv::Mat converted;
                    cv::cvtColor(m, converted, cv::COLOR_BGR2BGRA);
                    m = converted;
                }
            };
            to4ch(blackPart);
            to4ch(whitePart);

            cv::Mat combined;
            cv::hconcat(blackPart, canvas, combined);
            cv::hconcat(combined, whitePart, combined);

            cv::imshow("Kung Fu Chess - " + username, combined);
        };
        
        // ========== SECTION 13: MOUSE HANDLERS ==========
        mouse.setOnClick([&](const Position& pos) {
            controller.click(pos);
            redraw();
        });
        
        mouse.setOnRightClick([&](const Position& pos) {
            controller.jump(pos);
            redraw();
        });
        
        mouse.setOnOutsideClick([&]() {
            controller.clickOutside();
            redraw();
        });

        // ========== SECTION 14: SETUP WINDOW ==========
        cv::startWindowThread();
        cv::namedWindow("Kung Fu Chess - " + username, cv::WINDOW_NORMAL);
        redraw();

        mouse.attachTo("Kung Fu Chess - " + username);

        // ========== SECTION 15: MAIN GAME LOOP ==========
        std::cout << "\n🎮 Game started! Press ESC to quit\n" << std::endl;
        while (true)
        {
            int key = cv::waitKey(50) & 0xFF;           
            engine.update(50);
            redraw();
            
            if (key == 27) break;  // ESC
        }

        // ========== SECTION 16: CLEANUP ==========
        cv::destroyAllWindows();
        gameClient.disconnect();
        
        std::cout << "\n👋 Goodbye!" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "❌ Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}