#include "Game.hpp"

Game::Game(Board board) : board_(std::move(board)) {}

void Game::handleClick(int x, int y) {
    if (pendingMove_.has_value())
        {
            return;
        }

    std::optional<Position> cellOpt =
        ClickTranslator::pixelToCell(x, y, board_);    if (!cellOpt.has_value()) return; // click outside the board is ignored

    Position cell = *cellOpt;
    std::optional<Piece> pieceAtCell = board_.getCell(cell.row, cell.col);


    if (!selected_.has_value()) {
        if (pieceAtCell.has_value()) {
            selected_ = cell; // select the piece
        }
        // clicking an empty cell with no selection: ignored
        return;
    }

    Position selPos = *selected_;
    if (cell == selPos) {
        return; // clicking the already-selected cell: no-op
    }

    std::optional<Piece> selectedPiece = board_.getCell(selPos.row, selPos.col);

    if (pieceAtCell.has_value() && selectedPiece.has_value() &&
        sameColor(*pieceAtCell, *selectedPiece)) {
        // clicking another friendly piece replaces the selection
        selected_ = cell;
        return;
    }
    if (!selectedPiece->isValidShape(
            selPos.row,
            selPos.col,
            cell.row,
            cell.col))
    {
        return;
    }
    // clicking any other cell sends a move request from the
    // selected piece to that cell. No rules/duration yet, so the
    // move settles instantly.
    if (!board_.isValidMove(
        selPos.row,
        selPos.col,
        cell.row,
        cell.col))
{
    return;
}

int distance = std::max(
    std::abs(cell.row - selPos.row),
    std::abs(cell.col - selPos.col));

pendingMove_ = PendingMove{
    selPos,
    cell,
    *selectedPiece,
    clock_.now() + distance * 1000
};

selected_.reset();
}

void Game::handleWait(long ms) {
    clock_.advance(ms);
    
    if (pendingMove_.has_value() &&
    clock_.now() >= pendingMove_->arrivalTime)
    {
        board_.setCell(
            pendingMove_->to.row,
            pendingMove_->to.col,
            pendingMove_->piece);

        board_.setCell(
            pendingMove_->from.row,
            pendingMove_->from.col,
            std::nullopt);

        pendingMove_.reset();
    }
}

void Game::printBoard(std::ostream& out) const {
    board_.print(out);
}

bool Game::sameColor(const Piece& a, const Piece& b) {
    return a.color() == b.color();
}