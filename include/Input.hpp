#pragma once

#include <windows.h>

class Input {
public:
    static bool IsKeyDown(int virtual_key) {
        return (GetAsyncKeyState(virtual_key) & 0x8000) != 0;
    }
};
