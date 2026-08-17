#include "Glow.hpp"

#include "Game.hpp"
#include "Memory.hpp"
#include "Offsets.hpp"

#include <cstdint>

namespace {
constexpr std::uintptr_t kGlowPropertySize = 0x80;
constexpr std::uintptr_t kGlowColorOffset = 0x8;
constexpr std::uintptr_t kGlowAlphaOffset = 0x14;
constexpr std::uintptr_t kGlowTypeOffset = 0x30; // conservative placeholder
constexpr std::uintptr_t kGlowTeamOffset = 0x34;
constexpr std::uintptr_t kGlowColorOverrideOffset = 0x40;
constexpr std::uintptr_t kGlowEnabledOffset = 0x51;
constexpr std::uintptr_t kGlowHighlightOffset = 0x50;
}

bool Glow::enabled_ = false;
float Glow::red_ = 1.0f;
float Glow::green_ = 0.0f;
float Glow::blue_ = 1.0f;
float Glow::alpha_ = 0.6f;
bool Glow::through_walls_ = true;
std::uintptr_t Glow::last_glow_property_ = 0;
int Glow::last_glow_count_ = 0;

void Glow::Update() {
    if (!enabled_) {
        return;
    }

    const int local_team = Game::GetLocalTeam();
    for (int i = 1; i < 64; ++i) {
        const std::uintptr_t entity = Game::GetEntity(i);
        if (entity == 0 || !Game::IsValidTarget(entity, local_team)) {
            continue;
        }

        const std::uintptr_t glow_property = entity + Offsets::m_Glow;
        if (!Memory::IsReadable(glow_property, kGlowPropertySize)) {
            continue;
        }

        last_glow_property_ = glow_property;
        last_glow_count_++;

        Memory::Write<Vec3>(glow_property + kGlowColorOffset, {red_, green_, blue_});
        Memory::Write<float>(glow_property + kGlowAlphaOffset, alpha_);
        Memory::Write<int>(glow_property + kGlowTypeOffset, through_walls_ ? 2 : 0);
        Memory::Write<int>(glow_property + kGlowTeamOffset, 0);
        Memory::Write<Vec3>(glow_property + kGlowColorOverrideOffset, {red_, green_, blue_});
        Memory::Write<bool>(glow_property + kGlowHighlightOffset, true);
        Memory::Write<bool>(glow_property + kGlowEnabledOffset, true);
    }
}

bool Glow::IsEnabled() {
    return enabled_;
}

void Glow::SetEnabled(bool enabled) {
    enabled_ = enabled;
}

std::uintptr_t Glow::GetGlowPointer() {
    return last_glow_property_;
}

int Glow::GetGlowCount() {
    return last_glow_count_;
}

float Glow::GetRed() {
    return red_;
}

float Glow::GetGreen() {
    return green_;
}

float Glow::GetBlue() {
    return blue_;
}

float Glow::GetAlpha() {
    return alpha_;
}

void Glow::SetColor(float r, float g, float b, float a) {
    red_ = r;
    green_ = g;
    blue_ = b;
    alpha_ = a;
}

void Glow::SetThroughWalls(bool through_walls) {
    through_walls_ = through_walls;
}
