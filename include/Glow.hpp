#pragma once

#include <cstdint>

class Glow {
public:
    static void Update();

    static bool IsEnabled();
    static void SetEnabled(bool enabled);

    static float GetRed();
    static float GetGreen();
    static float GetBlue();
    static float GetAlpha();
    static void SetColor(float r, float g, float b, float a);

    static bool IsThroughWalls();
    static void SetThroughWalls(bool through_walls);

    static std::uintptr_t GetGlowPointer();
    static int GetGlowCount();

private:
    static bool enabled_;
    static float red_;
    static float green_;
    static float blue_;
    static float alpha_;
    static bool through_walls_;
    static std::uintptr_t last_glow_property_;
    static int last_glow_count_;
};
