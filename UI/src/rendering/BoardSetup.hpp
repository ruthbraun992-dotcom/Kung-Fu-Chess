#pragma once

#include "model/Board.hpp"
#include "Piece.hpp"

inline void setupStartingPosition(Board& board)
{
    // White
    board.setCell(0, 0, Piece(Piece::Color::BLACK, Piece::Type::ROOK));
    board.setCell(0, 1, Piece(Piece::Color::BLACK, Piece::Type::KNIGHT));
    board.setCell(0, 2, Piece(Piece::Color::BLACK, Piece::Type::BISHOP));
    board.setCell(0, 3, Piece(Piece::Color::BLACK, Piece::Type::QUEEN));
    board.setCell(0, 4, Piece(Piece::Color::BLACK, Piece::Type::KING));
    board.setCell(0, 5, Piece(Piece::Color::BLACK, Piece::Type::BISHOP));
    board.setCell(0, 6, Piece(Piece::Color::BLACK, Piece::Type::KNIGHT));
    board.setCell(0, 7, Piece(Piece::Color::BLACK, Piece::Type::ROOK));

    for (int c = 0; c < 8; ++c)
    {
        board.setCell(1, c, Piece(Piece::Color::BLACK, Piece::Type::PAWN));
    }

    // Black
    board.setCell(7, 0, Piece(Piece::Color::WHITE, Piece::Type::ROOK));
    board.setCell(7, 1, Piece(Piece::Color::WHITE, Piece::Type::KNIGHT));
    board.setCell(7, 2, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));
    board.setCell(7, 3, Piece(Piece::Color::WHITE, Piece::Type::QUEEN));
    board.setCell(7, 4, Piece(Piece::Color::WHITE, Piece::Type::KING));
    board.setCell(7, 5, Piece(Piece::Color::WHITE, Piece::Type::BISHOP));
    board.setCell(7, 6, Piece(Piece::Color::WHITE, Piece::Type::KNIGHT));
    board.setCell(7, 7, Piece(Piece::Color::WHITE, Piece::Type::ROOK));

    for(int c = 0; c < 8; ++c)
    {
        board.setCell(6, c, Piece(Piece::Color::WHITE, Piece::Type::PAWN));
    }
}