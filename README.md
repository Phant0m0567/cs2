# this was a test i was doing with quen code to see if ai could generate working cs2 cheats, i do not claim possesion of any of this code, this was just a sideproject i wanted to try.

# cs2-cheat


Windows x64 DLL with ImGui overlay, pattern scanning utilities, and aim helpers.

## Requirements

- Windows 10/11 x64
- Visual Studio 2022 (MSVC)
- CMake 3.20+
- CLion or Visual Studio

## Build

```bash
cmake -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Output: `build/bin/Release/cs2_math.dll`

CLion: open project, set toolchain to **Visual Studio x64**, build **Release**.

## Inject

1. Launch CS2 and reach the main menu.
2. Inject `cs2_math.dll` with any LoadLibrary injector.
3. Wait ~3 seconds for hooks to initialize.
4. Press **INSERT** to open the menu.

## Menu

| Tab | Contents |
|-----|----------|
| Aim | Enable aim, Aim X/Y smoothing, FOV |
| Status | Hook status, game ready state |
| Log | Injection and init messages |

**Aim key:** Mouse 5 (side button) held while aim is enabled.

## Offsets

CS2 updates break offsets. Before aim works, update `include/Offsets.hpp`:

```cpp
namespace Offsets {
    inline std::uintptr_t entity_list = 0x...;
    inline std::uintptr_t local_player = 0x...;
    inline std::uintptr_t view_angles = 0x...;
}
```

Use `Memory::FindPatternInModule("client.dll", "...")` to locate these after each game patch. The Status tab shows **Game ready: no** until offsets are valid.

## Aim X / Aim Y

- **Aim X** — horizontal (yaw) smoothing. Higher = slower, more human-like.
- **Aim Y** — vertical (pitch) smoothing.
- Value of `1.0` snaps instantly. Default is `4.0`.

## Project layout

```
include/   headers (Vec2, Vec3, Memory, Menu, Aim, Game, Offsets)
src/       implementation
```

## Disclaimer

For offline / `-insecure` local testing only. Do not use on VAC-secured servers.
