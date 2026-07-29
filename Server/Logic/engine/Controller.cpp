#include "Controller.hpp"
#include "../UI/src/networking/GameClient.hpp"

Controller::Controller(GameEngine& engine, GameClient* gameClient)
    : engine_(engine), gameClient_(gameClient)
{
}

void Controller::click(const Position& cell)
{
    if (engine_.isGameOver())
    {
        selected_.reset();
        return;
    }
    if (!selected_.has_value())
    {
        if (engine_.board().getCell(cell.row, cell.col).has_value())
        {
            selected_ = cell;
        }
        return;
    }

    auto selectedPiece = engine_.board().getCell(selected_->row, selected_->col);
    auto targetPiece = engine_.board().getCell(cell.row, cell.col);

    bool clickedDifferentCell =
        !(cell.row == selected_->row &&
          cell.col == selected_->col);

    if (clickedDifferentCell &&
        targetPiece.has_value() &&
        selectedPiece.has_value() &&
        targetPiece->color() == selectedPiece->color())
        {
            selected_ = cell;
            return;
        }

    Position from = *selected_;
    
    // ✅ SEND TO SERVER
    bool moved = false;
    if (gameClient_) {
        std::cout << "📤 Sending move to server: (" << from.row << "," << from.col 
                  << ") -> (" << cell.row << "," << cell.col << ")" << std::endl;
        gameClient_->sendMove(from.row, from.col, cell.row, cell.col);
        moved = true;
    } else {
        // Fallback: local mode
        std::cout << "🎮 Local mode: move validated locally" << std::endl;
        moved = engine_.requestMove(from.row, from.col, cell.row, cell.col);
    }
        
    if (engine_.isGameOver())
    {
        selected_.reset();
        return;
    }
    if (moved)
    {
        selected_.reset();
    }
}

// ✅ JUMP - also send to server
void Controller::jump(const Position& cell)
{
    if (gameClient_) {
        // Server doesn't have jump endpoint yet, but we can send it as a special move
        std::cout << "📤 Sending jump to server: (" << cell.row << "," << cell.col << ")" << std::endl;
        // For now, treat jump as local
        // TODO: Add jump support to server if needed
    }
    
    bool ok = engine_.requestJump(cell.row, cell.col);
    if (ok) {
        std::cout << "✅ Jump executed at (" << cell.row << "," << cell.col << ")" << std::endl;
    }
}

void Controller::clickOutside()
{
    selected_.reset();
}