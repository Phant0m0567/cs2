#pragma once

#include "Vec3.hpp"

#include <optional>
#include <cstdint>

struct ImVec2;

struct ViewAngles {
    float pitch = 0.0f;
    float yaw = 0.0f;
    float roll = 0.0f;
};

class Game {
public:
    static bool Initialize();
    static bool IsReady();

    static ViewAngles GetViewAngles();
    static void SetViewAngles(const ViewAngles& angles);
    static Vec3 GetLocalEyePosition();
    static int GetLocalTeam();

    static std::uintptr_t GetEntityListPtr();
    static std::uintptr_t GetLocalPlayerPtr();
    static std::uintptr_t GetEntity(int index);
    static std::optional<Vec3> GetEntityPosition(std::uintptr_t entity);
    static bool IsValidTarget(std::uintptr_t entity, int local_team);
    static std::optional<int> GetBestTargetIndex(float fov_degrees);
    static int GetLocalHealth();
    static int GetEntityHealth(std::uintptr_t entity);
    static int GetEntityTeam(std::uintptr_t entity);
    static bool IsEntityDormant(std::uintptr_t entity);
    static bool HasGunGameImmunity(std::uintptr_t entity);
    static bool IsEntityAlive(std::uintptr_t entity);
    static Vec3 GetLocalViewPunch();
    static std::uintptr_t GetActiveWeaponPtr();
    static bool IsOnGround();
    static std::uintptr_t GetPlantedC4Ptr();
    static bool IsBombTicking();
    static float GetBombTimeRemaining();
    static bool IsHostageCarried();
    static std::uintptr_t GetCarriedHostagePtr();

    static bool WorldToScreen(const Vec3& world_pos, ImVec2& screen_pos);

    static std::optional<Vec3> GetBestTarget(float fov_degrees);

private:
    static bool ResolvePointers();
    static bool GetViewMatrix(float matrix[16]);
    static std::uintptr_t client_base_;
    static std::uintptr_t entity_list_ptr_;
    static std::uintptr_t local_player_ptr_;
    static std::uintptr_t view_angles_ptr_;
    static bool ready_;
};
