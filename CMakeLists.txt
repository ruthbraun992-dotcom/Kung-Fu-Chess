cmake_minimum_required(VERSION 3.20)

project(KungFuChess)

set(CMAKE_CXX_STANDARD 17)

add_executable(KungFuChess
    main.cpp
    Board.cpp
    BoardParser.cpp
    Game.cpp
    IO.cpp
    Piece.cpp
)