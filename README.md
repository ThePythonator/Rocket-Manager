[![Build](https://github.com/ThePythonator/Rocket-Manager/actions/workflows/build.yml/badge.svg)](https://github.com/ThePythonator/Rocket-Manager/actions/workflows/build.yml)

# Rocket Manager
Create and fly rockets in a scaled-down solar system.
Created in limited time as an A-Level Computer Science NEA project.

### Running
The executable is located in the `bin/` directory when compiled as a release build.

Game save data can be found in:
- `data/saves/` for world save files
- `data/templates/` for rocket template files
Sharing these files is easy - just copy any shared files into the right directory

### Gameplay
Create rockets using the editor, by dragging and dropping components. If components touch, then they will automatically link together.
Once in-game, you can pause the game by pressing Escape or Space, select 'Load Rocket', and then choose which rocket to launch and where to launch from.

### Controls
Up/Down: Increase/Decrease engine throttle
Left/Right: Turn rocket
Space/Escape: Pause/Unpause
Z: change warp rate
Q/E: change currently selected/controlled rocket
M: open/close main map

### Tools Used
- C++ for all code
- CMake to build the project
- SDL2 for graphics and input
- Aseprite for creating the font