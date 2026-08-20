#include "Game.hpp"

#include "Memory.hpp"
#include "Menu.hpp"
#include "Offsets.hpp"
#include "Vec3.hpp"

#include <algorithm>
#include <imgui.h>
#include <cmath>
#include <cstdio>

namespace {
constexpr float k_pi = 3.14159265358979323846f;
}

std::uintptr_t Game::client_base_ = 0;
std::uintptr_t Game::entity_list_ptr_ = 0;
std::uintptr_t Game::local_player_ptr_ = 0;
std::uintptr_t Game::view_angles_ptr_ = 0;
bool Game::ready_ = false;

std::uintptr_t Game::GetEntityListPtr() {
    return entity_list_ptr_;
}

std::uintptr_t Game::GetLocalPlayerPtr() {
    return local_player_ptr_;
}

std::uintptr_t Game::GetEntity(int index) {
    if (entity_list_ptr_ == 0 || index < 0 || index >= 64) {
        return 0;
    }

    const std::uintptr_t entity = Memory::Read<std::uintptr_t>(entity_list_ptr_ + static_cast<std::uintptr_t>(index) * 0x10);
    if (entity != 0) {
        return entity;
    }

    const std::uintptr_t fallback_list = Memory::Read<std::uintptr_t>(entity_list_ptr_);
    if (fallback_list == 0) {
        return 0;
    }

    return Memory::Read<std::uintptr_t>(fallback_list + static_cast<std::uintptr_t>(index) * 0x10);
}

bool Game::Initialize() {
    client_base_ = Memory::GetModuleBase("client.dll");
    if (client_base_ == 0) {
        Menu::AddLog("Game::Initialize: client.dll not found");
        return false;
    }

    Menu::AddLog("Game::Initialize: client.dll found");
    return ResolvePointers();
}

bool Game::ResolvePointers() {
    if (Offsets::entity_list != 0) {
        entity_list_ptr_ = client_base_ + Offsets::entity_list;
    }

    if (Offsets::local_player != 0) {
        local_player_ptr_ = Memory::Read<std::uintptr_t>(client_base_ + Offsets::local_player);
    }

    if (Offsets::view_angles != 0) {
        view_angles_ptr_ = client_base_ + Offsets::view_angles;
    }

    ready_ = local_player_ptr_ != 0 && view_angles_ptr_ != 0 && entity_list_ptr_ != 0;
    if (!ready_) {
        Menu::AddLog("Game::ResolvePointers: pointers invalid");
    } else {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer),
            "Game::ResolvePointers: entity=0x%llx local=0x%llx view=0x%llx",
            static_cast<unsigned long long>(entity_list_ptr_),
            static_cast<unsigned long long>(local_player_ptr_),
            static_cast<unsigned long long>(view_angles_ptr_));
        Menu::AddLog(buffer);
    }

    return ready_;
}

bool Game::IsReady() {
    if (!ready_) {
        ResolvePointers();
    }

    return ready_ && local_player_ptr_ != 0;
}

ViewAngles Game::GetViewAngles() {
    if (view_angles_ptr_ == 0) {
        return {};
    }

    return Memory::Read<ViewAngles>(view_angles_ptr_);
}

void Game::SetViewAngles(const ViewAngles& angles) {
    if (view_angles_ptr_ == 0) {
        return;
    }

    ViewAngles clamped = angles;
    if (clamped.pitch > 89.0f) {
        clamped.pitch = 89.0f;
    }
    if (clamped.pitch < -89.0f) {
        clamped.pitch = -89.0f;
    }

    Memory::Write<ViewAngles>(view_angles_ptr_, clamped);
}

Vec3 Game::GetLocalEyePosition() {
    if (local_player_ptr_ == 0) {
        return {};
    }

    const Vec3 origin = Memory::Read<Vec3>(local_player_ptr_ + Offsets::m_vec_origin);
    Vec3 view_offset{};
    if (Offsets::m_vec_view_offset != 0 && Memory::IsReadable(local_player_ptr_ + Offsets::m_vec_view_offset, sizeof(Vec3))) {
        view_offset = Memory::Read<Vec3>(local_player_ptr_ + Offsets::m_vec_view_offset);
    }

    return origin + view_offset;
}

int Game::GetLocalTeam() {
    if (local_player_ptr_ == 0) {
        return 0;
    }

    return static_cast<int>(Memory::Read<std::uint8_t>(local_player_ptr_ + Offsets::m_i_team_num));
}

bool Game::GetViewMatrix(float matrix[16]) {
    if (Offsets::view_matrix == 0 || client_base_ == 0) {
        return false;
    }

    const std::uintptr_t view_matrix_address = client_base_ + Offsets::view_matrix;
    if (!Memory::IsReadable(view_matrix_address, sizeof(float) * 16)) {
        return false;
    }

    for (int i = 0; i < 16; ++i) {
        matrix[i] = Memory::Read<float>(view_matrix_address + sizeof(float) * i);
    }
    return true;
}

int Game::GetLocalHealth() {
    if (local_player_ptr_ == 0) {
        return 0;
    }

    return Memory::Read<int>(local_player_ptr_ + Offsets::m_i_health);
}

int Game::GetEntityHealth(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_i_health, sizeof(int))) {
        return 0;
    }

    return Memory::Read<int>(entity + Offsets::m_i_health);
}

int Game::GetEntityTeam(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_i_team_num, sizeof(std::uint8_t))) {
        return 0;
    }

    return static_cast<int>(Memory::Read<std::uint8_t>(entity + Offsets::m_i_team_num));
}

int Game::GetEntityArmor(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_i_armor_value, sizeof(int))) {
        return 0;
    }
    return Memory::Read<int>(entity + Offsets::m_i_armor_value);
}

std::uint32_t Game::GetEntityWeaponHash(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_unWeaponHash, sizeof(std::uint32_t))) {
        return 0;
    }
    return Memory::Read<std::uint32_t>(entity + Offsets::m_unWeaponHash);
}

bool Game::IsEntitySpotted(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_entity_spotted_state, sizeof(std::uint32_t))) {
        return false;
    }
    return Memory::Read<std::uint32_t>(entity + Offsets::m_entity_spotted_state) != 0;
}

bool Game::IsEntityDormant(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_bDormant, sizeof(bool))) {
        return false;
    }
    return Memory::Read<bool>(entity + Offsets::m_bDormant);
}

bool Game::HasGunGameImmunity(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_bGunGameImmunity, sizeof(bool))) {
        return false;
    }
    return Memory::Read<bool>(entity + Offsets::m_bGunGameImmunity);
}

bool Game::IsEntityAlive(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_i_health, sizeof(int))) {
        return false;
    }
    const int health = Memory::Read<int>(entity + Offsets::m_i_health);
    return health > 0;
}

Vec3 Game::GetLocalViewPunch() {
    if (local_player_ptr_ == 0 || !Memory::IsReadable(local_player_ptr_ + Offsets::m_pCameraServices, sizeof(std::uintptr_t))) {
        return {};
    }

    const std::uintptr_t camera_services = Memory::Read<std::uintptr_t>(local_player_ptr_ + Offsets::m_pCameraServices);
    if (camera_services == 0 || !Memory::IsReadable(camera_services + Offsets::m_vec_cs_view_punch, sizeof(Vec3))) {
        return {};
    }

    return Memory::Read<Vec3>(camera_services + Offsets::m_vec_cs_view_punch);
}

std::uintptr_t Game::GetActiveWeaponPtr() {
    if (local_player_ptr_ == 0 || !Memory::IsReadable(local_player_ptr_ + Offsets::m_pWeaponServices, sizeof(std::uintptr_t))) {
        return 0;
    }

    const std::uintptr_t weapon_services = Memory::Read<std::uintptr_t>(local_player_ptr_ + Offsets::m_pWeaponServices);
    if (weapon_services == 0 || !Memory::IsReadable(weapon_services + Offsets::m_hActiveWeapon, sizeof(std::uintptr_t))) {
        return 0;
    }

    const std::uintptr_t handle = Memory::Read<std::uintptr_t>(weapon_services + Offsets::m_hActiveWeapon);
    if (handle == 0) {
        return 0;
    }

    const int index = static_cast<int>(handle & 0xFFF);
    if (index <= 0 || index >= 64) {
        return 0;
    }

    return GetEntity(index);
}

int Game::GetLocalWeaponClip() {
    const std::uintptr_t weapon = GetActiveWeaponPtr();
    if (weapon == 0 || !Memory::IsReadable(weapon + Offsets::m_iClip1, sizeof(int))) {
        return 0;
    }
    return Memory::Read<int>(weapon + Offsets::m_iClip1);
}

int Game::GetLocalWeaponReserve() {
    const std::uintptr_t weapon = GetActiveWeaponPtr();
    if (weapon == 0 || !Memory::IsReadable(weapon + Offsets::m_pReserveAmmo, sizeof(int))) {
        return 0;
    }
    return Memory::Read<int>(weapon + Offsets::m_pReserveAmmo);
}

bool Game::IsOnGround() {
    if (local_player_ptr_ == 0 || !Memory::IsReadable(local_player_ptr_ + Offsets::m_fFlags, sizeof(std::uint32_t))) {
        return false;
    }

    const std::uint32_t flags = Memory::Read<std::uint32_t>(local_player_ptr_ + Offsets::m_fFlags);
    return (flags & 1u) != 0;
}

std::uintptr_t Game::GetPlantedC4Ptr() {
    if (client_base_ == 0 || Offsets::planted_c4 == 0) {
        return 0;
    }

    const std::uintptr_t planted_c4_ptr = Memory::Read<std::uintptr_t>(client_base_ + Offsets::planted_c4);
    if (planted_c4_ptr == 0) {
        return 0;
    }

    return planted_c4_ptr;
}

bool Game::IsBombTicking() {
    const std::uintptr_t bomb_ptr = GetPlantedC4Ptr();
    if (bomb_ptr == 0 || !Memory::IsReadable(bomb_ptr + Offsets::m_bBombTicking, sizeof(bool))) {
        return false;
    }

    return Memory::Read<bool>(bomb_ptr + Offsets::m_bBombTicking);
}

float Game::GetBombTimeRemaining() {
    const std::uintptr_t bomb_ptr = GetPlantedC4Ptr();
    if (bomb_ptr == 0) {
        return 0.0f;
    }

    if (Memory::IsReadable(bomb_ptr + Offsets::m_flTimerLength, sizeof(float))) {
        return Memory::Read<float>(bomb_ptr + Offsets::m_flTimerLength);
    }

    if (Memory::IsReadable(bomb_ptr + Offsets::m_flC4Blow, sizeof(float))) {
        return Memory::Read<float>(bomb_ptr + Offsets::m_flC4Blow);
    }

    return 0.0f;
}

std::uintptr_t Game::GetCarriedHostagePtr() {
    if (local_player_ptr_ == 0 || !Memory::IsReadable(local_player_ptr_ + Offsets::m_pHostageServices, sizeof(std::uintptr_t))) {
        return 0;
    }

    const std::uintptr_t hostage_service = Memory::Read<std::uintptr_t>(local_player_ptr_ + Offsets::m_pHostageServices);
    if (hostage_service == 0 || !Memory::IsReadable(hostage_service + Offsets::m_hCarriedHostage, sizeof(std::uintptr_t))) {
        return 0;
    }

    const std::uintptr_t handle = Memory::Read<std::uintptr_t>(hostage_service + Offsets::m_hCarriedHostage);
    if (handle == 0) {
        return 0;
    }

    const int index = static_cast<int>(handle & 0xFFF);
    if (index > 0 && index < 64) {
        return GetEntity(index);
    }

    return handle;
}

bool Game::IsHostageCarried() {
    return GetCarriedHostagePtr() != 0;
}

std::optional<int> Game::GetBestTargetIndex(float fov_degrees) {
    if (!IsReady() || entity_list_ptr_ == 0) {
        return std::nullopt;
    }

    const Vec3 eye = GetLocalEyePosition();
    const ViewAngles view = GetViewAngles();
    const int local_team = GetLocalTeam();

    std::optional<int> best_index{};
    float best_fov = fov_degrees;

    for (int i = 1; i < 64; ++i) {
        const std::uintptr_t entity = GetEntity(i);
        if (entity == 0) {
            continue;
        }

        if (!IsValidTarget(entity, local_team)) {
            continue;
        }

        const auto position = GetEntityPosition(entity);
        if (!position.has_value()) {
            continue;
        }

        Vec3 aim_point = *position;
        aim_point.z += 64.0f;

        const float fov = GetFovToTarget(view, eye, aim_point);
        if (fov < best_fov) {
            best_fov = fov;
            best_index = i;
        }
    }

    return best_index;
}

bool Game::WorldToScreen(const Vec3& world_pos, ImVec2& screen_pos) {
    const ImGuiIO& io = ImGui::GetIO();
    if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f) {
        return false;
    }

    float matrix[16];
    if (GetViewMatrix(matrix)) {
        const float clip_x = world_pos.x * matrix[0] + world_pos.y * matrix[1] + world_pos.z * matrix[2] + matrix[3];
        const float clip_y = world_pos.x * matrix[4] + world_pos.y * matrix[5] + world_pos.z * matrix[6] + matrix[7];
        const float clip_w = world_pos.x * matrix[12] + world_pos.y * matrix[13] + world_pos.z * matrix[14] + matrix[15];

        if (clip_w <= 0.001f) {
            return false;
        }

        const float inv_w = 1.0f / clip_w;
        screen_pos.x = (io.DisplaySize.x * 0.5f) + (clip_x * inv_w) * (io.DisplaySize.x * 0.5f);
        screen_pos.y = (io.DisplaySize.y * 0.5f) - (clip_y * inv_w) * (io.DisplaySize.y * 0.5f);
        return true;
    }

    if (!IsReady()) {
        return false;
    }

    const Vec3 eye = GetLocalEyePosition();
    const ViewAngles view = GetViewAngles();
    const Vec3 delta = world_pos - eye;
    const ViewAngles target_angles = DirectionToAngles(delta);

    const float yaw_delta = NormalizeYaw(target_angles.yaw - view.yaw);
    const float pitch_delta = target_angles.pitch - view.pitch;

    constexpr float screen_fov = 90.0f;
    const float half_fov = screen_fov * 0.5f;
    if (std::fabs(yaw_delta) > half_fov || std::fabs(pitch_delta) > half_fov) {
        return false;
    }

    screen_pos.x = (io.DisplaySize.x * 0.5f) + (yaw_delta / half_fov) * (io.DisplaySize.x * 0.5f);
    screen_pos.y = (io.DisplaySize.y * 0.5f) - (pitch_delta / half_fov) * (io.DisplaySize.y * 0.5f);

    return true;
}

std::optional<Vec3> Game::GetEntityPosition(std::uintptr_t entity) {
    if (entity == 0 || !Memory::IsReadable(entity + Offsets::m_vec_origin, sizeof(Vec3))) {
        return std::nullopt;
    }

    return Memory::Read<Vec3>(entity + Offsets::m_vec_origin);
}

bool Game::IsValidTarget(std::uintptr_t entity, int local_team) {
    if (entity == 0 || entity == local_player_ptr_) {
        return false;
    }

    if (IsEntityDormant(entity) || HasGunGameImmunity(entity) || !IsEntityAlive(entity)) {
        return false;
    }

    const int team = GetEntityTeam(entity);
    if (team == local_team || team == 0) {
        return false;
    }

    return true;
}

static ViewAngles DirectionToAngles(const Vec3& direction) {
    const float hyp = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    ViewAngles angles{};
    angles.pitch = std::atan2(-direction.z, hyp) * (180.0f / k_pi);
    angles.yaw = std::atan2(direction.y, direction.x) * (180.0f / k_pi);
    return angles;
}

static float NormalizeYaw(float yaw) {
    while (yaw > 180.0f) {
        yaw -= 360.0f;
    }
    while (yaw < -180.0f) {
        yaw += 360.0f;
    }
    return yaw;
}

static float AngleDelta(float a, float b) {
    return NormalizeYaw(b - a);
}

static float GetFovToTarget(const ViewAngles& view, const Vec3& eye, const Vec3& target) {
    const Vec3 delta = target - eye;
    const ViewAngles target_angles = DirectionToAngles(delta);

    const float pitch_delta = target_angles.pitch - view.pitch;
    const float yaw_delta = AngleDelta(view.yaw, target_angles.yaw);

    return std::sqrt(pitch_delta * pitch_delta + yaw_delta * yaw_delta);
}

std::optional<Vec3> Game::GetBestTarget(float fov_degrees) {
    if (!IsReady() || entity_list_ptr_ == 0) {
        return std::nullopt;
    }

    const Vec3 eye = GetLocalEyePosition();
    const ViewAngles view = GetViewAngles();
    const int local_team = GetLocalTeam();

    std::optional<Vec3> best_target{};
    float best_fov = fov_degrees;

    const std::uintptr_t list_base = Memory::Read<std::uintptr_t>(entity_list_ptr_);
    if (list_base == 0) {
        return std::nullopt;
    }
    if (!Memory::IsReadable(entity_list_ptr_, sizeof(std::uintptr_t) * 64)) {
        return std::nullopt;
    }
    for (int i = 1; i < 64; ++i) {
        const std::uintptr_t entity = Memory::Read<std::uintptr_t>(list_base + (i * 0x10));
        if (entity == 0) {
            continue;
        }

        if (!IsValidTarget(entity, local_team)) {
            continue;
        }

        const auto position = GetEntityPosition(entity);
        if (!position.has_value()) {
            continue;
        }

        Vec3 aim_point = *position;
        aim_point.z += 64.0f;

        const float fov = GetFovToTarget(view, eye, aim_point);
        if (fov < best_fov) {
            best_fov = fov;
            best_target = aim_point;
        }
    }

    return best_target;
}
