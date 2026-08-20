# cs2-cheat

Windows x64 DLL cheat for CS2 with an ImGui overlay and in-game features.

## How it works

- The DLL is injected into CS2 and hooks DirectX11 rendering.
- The overlay renders menu controls and ESP text/boxes.
- Game state is read from client memory using hardcoded offsets in `include/Offsets.hpp`.
- Aim features work by reading local view angles and writing adjusted view angles in-game.
- Config profiles are saved to `cs2_math_<profile>.cfg` and loaded from the same folder.

## Requirements

- Windows 10/11 x64
- Visual Studio 2022 or newer (MSVC)
- CMake 3.20+
- CS2 x64

## Build

```bash
cmake -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Output DLL:

- `build/bin/Release/cs2_math.dll`

Alternatively, open the project in Visual Studio or CLion and build the `Release` configuration.

## Run / Inject

1. Start CS2 and wait for the main menu or a match.
2. Inject `cs2_math.dll` using an external DLL injector that supports `LoadLibrary`.
3. Wait a few seconds for hooks and pointers to initialize.
4. Press **INSERT** to open the cheat menu.

## Menu overview

- `Aim` tab: enable aimbot, smoothing, FOV, triggers, and no-recoil/spread options.
- `Status` tab: shows game readiness, active target info, bomb/hostage state, and current hook status.
- `Log` tab: displays injection and initialization messages.
- `Settings` tab: profile save/load, visible-only ESP, weapon info, distance, and overlay toggles.

## Offsets

CS2 updates can break pointers. Update `include/Offsets.hpp` when the game changes.

Key entries:

```cpp
namespace Offsets {
    inline std::uintptr_t entity_list = 0x...;
    inline std::uintptr_t local_player = 0x...;
    inline std::uintptr_t view_angles = 0x...;
}
```

When offsets are invalid, the `Status` tab will usually show `Game ready: no`.

## Features

- Aimbot with smooth aim and rage modes
- Triggerbot and rapid-fire support
- No recoil, no spread, and no-scope inaccuracy removal
- ESP boxes with health, armor, weapon hash, and distance
- Visible-only ESP filtering
- Bomb and hostage overlay indicators
- Profile-based config save/load

## Project structure

```
include/   headers (Aim, Game, Memory, Menu, Offsets, Vec2, Vec3)
src/       implementation files
```

## Disclaimer

For offline or local testing only. Do not use on VAC-secured servers.
