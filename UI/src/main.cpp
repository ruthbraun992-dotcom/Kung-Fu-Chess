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

#include <filesystem>
#include <iostream>
#include <optional>
#include <vector>

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
        if (parent == current)
        {
            break;
        }

        current = parent;
    }

    candidates.push_back(std::filesystem::current_path() / relativePath);

    for (const auto& candidate : candidates)
    {
        const std::filesystem::path normalized = candidate.lexically_normal();
        if (std::filesystem::exists(normalized))
        {
            return normalized;
        }
    }

    return candidates.front().lexically_normal();
}
}

int main()
{
    try
    {
        const std::filesystem::path boardImagePath ="C:\\Users\\This User\\Desktop\\Kung-Fu Chess\\UI\\pieces6\\board.png";
         //  "C:\\Users\\This User\\Downloads\\board.png";//

        Img boardImage;
        Img gameOverImage;
        
        const auto gameOverPath = resolveAssetPath("../GameOver.png");
        gameOverImage.read(gameOverPath.string(), {800, 800}, false);
        boardImage.read(boardImagePath.string(), {650, 650}, false);
        const std::filesystem::path spriteDir = resolveAssetPath("../pieces6");
        std::cout << "spriteDir = " << spriteDir << " exists=" << std::filesystem::exists(spriteDir) << std::endl;
        constexpr int kBoardSize = 960;
        const int cellSize = 63;//kBoardSize / 8;
        const int offsetX =90;
        const int offsetY =90;

        const cv::Mat& img = boardImage.get_mat();

      Board board(8, 8);
    setupStartingPosition(board);

    AnimationConfigLoader animConfigs(spriteDir.string());
    GameEngine engine(std::move(board), animConfigs);
    Controller controller(engine);

    SpriteManager sprites(spriteDir.string());
    BoardRenderer renderer(8,  8,  cellSize,   sprites, offsetX,  offsetY,engine);
    MovesLogRenderer logRenderer(250, 800, 8, 8);

        cv::Mat canvas;

        const int logColWidth = 250;

ClickTranslator translator(
    8,
    8,
    cellSize,
    offsetX + logColWidth, 
    offsetY);

        MouseHandler mouse(translator);

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

    cv::imshow("Image", combined);
};mouse.setOnClick([&](const Position& pos)
        {
            controller.click(pos);
            redraw();
        });
        mouse.setOnRightClick([&](const Position& pos)
        {
                std::cout << "right click lambda: pos=" << pos.row << "," << pos.col << std::endl;
        controller.jump(pos);
        redraw();
        });
        mouse.setOnOutsideClick([&]()
        {
            controller.clickOutside();
            redraw();
        });

        cv::startWindowThread();
        cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
        cv::namedWindow("Image", cv::WINDOW_NORMAL);
        redraw();

        mouse.attachTo("Image");

        while (true)
        {
            int key = cv::waitKey(50) & 0xFF;           
            engine.update(50);
            auto pos =engine.currentPositionOf({7,2});
            redraw();
            

            if (key == 27)
                break;
        }

        cv::destroyAllWindows();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}