# Kung-Fu-Chess

A real-time chess variant where pieces move continuously over time instead of
turn-by-turn. Each piece has its own movement duration based on speed, so
multiple pieces can be in motion simultaneously — there are no turns.

## Architecture

The project is split into independent layers:

Common/ # Shared, dependency-free structs (Piece, AnimationConfig, RenderPosition)
Logic/ # Core game logic — zero OpenCV dependency, fully unit-testable
model/ Board state
rules/ Move validation (PieceRules, RuleEngine, BoardMapper, BoardParser)
realtime/ RealTimeArbiter — manages in-flight motions and timing
engine/ GameEngine, Controller — orchestrates a full game
stats/ GameStats — score tracking, move history
animation/ AnimationConfigLoader — reads per-state animation config (no images)
UI/ # OpenCV-based graphical client
src/
input/ ClickTranslator, MouseHandler
rendering/ BoardRenderer, MovesLogRenderer, board layout constants
sprites/ SpriteManager, image loading
tests/ # Catch2 unit tests
unit/


`Logic/` has no dependency on OpenCV or any UI framework — it can be built and
tested in complete isolation. `UI/` depends on `Logic/` and OpenCV to render
and handle input.

## Requirements

- CMake 3.14+
- A C++17 compiler (tested with MSVC / Visual Studio 2022)
- OpenCV 4.5.1 (bundled under `UI/OpenCV_451`)

## Build

```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

> **Note:** if the executable fails to start with a missing-DLL error, copy
> `opencv_world451d.dll` (Debug) or `opencv_world451.dll` (Release) from
> `UI/OpenCV_451/bin` into the same folder as `kungfu_chess.exe`.

## Run

After building, run the executable produced under `build/UI/`
(`kungfu_chess.exe`). The client opens an OpenCV window showing the board;
click a piece to select it, then click a destination square to move it.
Right-click triggers a jump, where supported.

## Tests

Unit tests use Catch2 and are discovered automatically via CTest:

```powershell
cd build
ctest
```

## Project status

| Area | Feature | Status |
|---|---|---|
| Logic | Board parsing & piece model | ✅ |
| Logic | All piece movement rules (K, Q, R, B, N, P) | ✅ |
| Logic | Invalid moves rejected | ✅ |
| Logic | Blocking & capture rules | ✅ |
| Logic | Real-time movement (arrival-based board updates) | ✅ |
| Logic | King capture ends the game | ✅ |
| Graphics | Transparent piece sprites | ✅ |
| Graphics | Move/jump/rest animations | ✅ |
| Graphics | Local playable client (mouse input) | ✅ |
| Graphics | Selection / error / game-over display | ✅ In progress |
| Server | WebSocket server | 🚧 Not started |
| Server | Room creation/joining | 🚧 Not started |
| Server | White/black assignment | 🚧 Not started |
| Server | Server-side move validation | 🚧 Not started |
| Server | State broadcast to clients | 🚧 Not started |
| Server | Event bus (pub/sub) | 🚧 Designed, not yet implemented |
| Server | Disconnect/resign handling | 🚧 Not started |

## Roadmap

1. Event bus (pub/sub) — scores, move logs, sounds, game start/end animations
2. Single-process WebSocket server
3. Home screen with shell-based login
4. User accounts + ELO rating (SQLite)
5. Matchmaking
6. Disconnect handling with auto-resign
7. Rooms (create/join/viewers)
8. Client/server activity logging
