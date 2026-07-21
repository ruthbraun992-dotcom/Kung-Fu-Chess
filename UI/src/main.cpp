#include "img.hpp"
#include "MouseHandler.hpp"
#include "ClickTranslator.hpp"
#include "SpriteManager.hpp"
#include "BoardRenderer.hpp"
#include "BoardSetup.hpp"
#include "Board.hpp"
#include "position.hpp"
#include "GameEngine.hpp"
#include "Controller.hpp"


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
        const std::filesystem::path boardImagePath = "C:\\Users\\This User\\Desktop\\Kung-Fu Chess\\UI\\pieces6\\board.png";
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
        const int offsetX = 90;
        const int offsetY = 90;

        const cv::Mat& img = boardImage.get_mat();

      Board board(8, 8);
    setupStartingPosition(board);

    AnimationConfigLoader animConfigs(spriteDir.string());
    GameEngine engine(std::move(board), animConfigs);
    Controller controller(engine);

    SpriteManager sprites(spriteDir.string());
    BoardRenderer renderer(8,  8,  cellSize,   sprites, offsetX,  offsetY,engine);

        cv::Mat canvas;

        ClickTranslator translator(
            8,
            8,
            cellSize,
            offsetX,
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

    cv::imshow("Image", canvas);
    };

        mouse.setOnClick([&](const Position& pos)
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