#include "Glow.hpp"

#include "Game.hpp"
#include "Memory.hpp"
#include "Offsets.hpp"

#include <cstdint>

namespace {
constexpr std::uintptr_t kGlowObjectSize = 0x38;
}

bool Glow::enabled_ = false;
float Glow::red_ = 1.0f;
float Glow::green_ = 0.0f;
float Glow::blue_ = 1.0f;
float Glow::alpha_ = 0.6f;
bool Glow::through_walls_ = true;
std::uintptr_t Glow::last_glow_manager_ = 0;
int Glow::last_glow_count_ = 0;

void Glow::Update() {
    if (!enabled_) {
        return;
    }

    const std::uintptr_t client_base = Memory::GetModuleBase("client.dll");
    if (client_base == 0 || Offsets::glow_object_manager == 0 || Offsets::m_iGlowIndex == 0) {
        return;
    }

    const std::uintptr_t glow_manager_ptr = client_base + Offsets::glow_object_manager;
    if (!Memory::IsReadable(glow_manager_ptr, sizeof(std::uintptr_t))) {
        return;
    }

    const std::uintptr_t glow_manager = Memory::Read<std::uintptr_t>(glow_manager_ptr);
    if (glow_manager == 0 || !Memory::IsReadable(glow_manager, sizeof(std::uintptr_t) + sizeof(int))) {
        return;
    }

    const std::uintptr_t glow_object_array = Memory::Read<std::uintptr_t>(glow_manager);
    const int glow_count = Memory::Read<int>(glow_manager + sizeof(std::uintptr_t));
    last_glow_manager_ = glow_manager;
    last_glow_count_ = glow_count;
    if (glow_object_array == 0 || glow_count <= 0 || glow_count > 1024) {
        return;
    }

    const int local_team = Game::GetLocalTeam();

    for (int i = 1; i < 64; ++i) {
        const std::uintptr_t entity = Game::GetEntity(i);
        if (entity == 0 || !Game::IsValidTarget(entity, local_team)) {
            continue;
        }

        const int glow_index = Memory::Read<int>(entity + Offsets::m_iGlowIndex);
        if (glow_index < 0 || glow_index >= glow_count) {
            continue;
        }

        const std::uintptr_t glow_object = glow_object_array + static_cast<std::uintptr_t>(glow_index) * kGlowObjectSize;
        if (!Memory::IsReadable(glow_object, kGlowObjectSize)) {
            continue;
        }

        Memory::Write<float>(glow_object + 0x8, red_);
        Memory::Write<float>(glow_object + 0xC, green_);
        Memory::Write<float>(glow_object + 0x10, blue_);
        Memory::Write<float>(glow_object + 0x14, alpha_);
        Memory::Write<uint8_t>(glow_object + 0x18, through_walls_ ? 1 : 0);
        Memory::Write<uint8_t>(glow_object + 0x19, 1);
        Memory::Write<uint8_t>(glow_object + 0x1A, 0);
        Memory::Write<uint8_t>(glow_object + 0x1B, 0);
    }
}

bool Glow::IsEnabled() {
    return enabled_;
}

void Glow::SetEnabled(bool enabled) {
    enabled_ = enabled;
}

std::uintptr_t Glow::GetGlowManagerPtr() {
    return last_glow_manager_;
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
