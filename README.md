# Kung-Fu-Chess

A real-time chess variant where pieces move continuously over time instead of
turn-by-turn. Each piece has its own movement duration based on speed, so
multiple pieces can be in motion simultaneously - there are no turns.

The project includes a local OpenCV client, a WebSocket game server, and
unit tests.

## Architecture

The project is split into independent layers:

```
```
Common/       Shared, dependency-free structs (Piece, AnimationConfig, RenderPosition)
Server/       WebSocket game server
  Logic/        Core game logic (board, rules, real-time arbiter, game engine, stats)
  main.cpp      Server entry point
  WebSocketServer, SessionManager, GameManager, RoomManager,
  MatchmakingQueue, UserRepository, PasswordHasher, Logger
UI/           OpenCV-based graphical client (input, rendering, sprites)
tests/        Catch2 unit tests
ThirdParty/   Third-party libraries (e.g. picosha2)
asio/         Git submodule (asio 1.28.0)
websocketpp/  Git submodule
```

The game logic does not depend on the UI, so it can be built and tested in
isolation.

## Requirements

- Git
- CMake 3.14+
- A C++17 compiler (tested with MSVC / Visual Studio 2022)
- OpenCV 4.5.1 (bundled under `UI/OpenCV_451`)

## Getting the code

`asio` and `websocketpp` are Git submodules, so clone with:

```powershell
git clone --recurse-submodules https://github.com/ruthbraun992-dotcom/Kung-Fu-Chess.git
cd Kung-Fu-Chess
```

If you already cloned without `--recurse-submodules`, run:

```powershell
git submodule update --init
```

## Build

```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

> **Note:** if the executable fails to start with a missing-DLL error, copy
> `opencv_world451d.dll` (Debug) or `opencv_world451.dll` (Release) from
> `UI/OpenCV_451/bin` into the same folder as the executable.

## Run

1. Start the server (executable produced under `build/Server/`).
2. Start one client per player (`kungfu_chess.exe`, produced under `build/UI/`).

The client opens an OpenCV window showing the board. Click a piece to select
it, then click a destination square to move it. Right-click triggers a jump,
where supported.

## Tests

Unit tests use Catch2 and are discovered automatically via CTest:

```powershell
cd build
ctest -C Debug
```

## Project status

| Area | Feature | Status |
|---|---|---|
| Logic | Board parsing & piece model | Done |
| Logic | All piece movement rules (K, Q, R, B, N, P) | Done |
| Logic | Blocking & capture rules | Done |
| Logic | Real-time movement (arrival-based board updates) | Done |
| Logic | King capture ends the game | Done |
| Graphics | Transparent piece sprites | Done |
| Graphics | Move/jump/rest animations | Done |
| Graphics | Local playable client (mouse input) | Done |
| Server | WebSocket server | Done (basic) |
| Server | Two players in the same game | Done |
| Server | Score per player | Done |
| Server | UI connected to server | Done |
| Server | Rooms | Done (basic) |
| Server | Matchmaking queue | Done (basic) |
| Server | User accounts (SQLite, hashed passwords) | Done (basic) |
| Server | Game-over handling | In progress |
| Server | Event bus (pub/sub) | Planned |
| Server | Disconnect/resign handling | Planned |

## Roadmap

1. Event bus (pub/sub) - scores, move logs, sounds, game start/end animations
2. ELO rating
3. Disconnect handling with auto-resign
4. Viewers (spectators) in rooms
5. Client/server activity logging
