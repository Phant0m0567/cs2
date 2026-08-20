#include "Aim.hpp"

#include "Game.hpp"
#include "Input.hpp"
#include "Memory.hpp"
#include "Vec3.hpp"

#include <imgui.h>
#include <cmath>
#include <cstdint>
#include <chrono>
#include <string>

namespace {
constexpr float k_pi = 3.14159265358979323846f;
constexpr std::uint32_t k_trigger_min_interval_ms = 80;
}

bool Aim::enabled_ = false;
bool Aim::master_enabled_ = true;
bool Aim::wallbang_enabled_ = false;
bool Aim::bunnyhop_enabled_ = false;
bool Aim::esp_enabled_ = true;
bool Aim::spinbot_enabled_ = false;
float Aim::spinbot_speed_ = 6.0f;
bool Aim::triggerbot_enabled_ = false;
float Aim::triggerbot_fov_ = 3.0f;
bool Aim::ragebot_enabled_ = false;
bool Aim::legit_mode_enabled_ = true;
Aim::Bone Aim::aim_bone_ = Aim::Bone::Head;
bool Aim::auto_stop_enabled_ = true;
bool Aim::auto_scope_enabled_ = false;
bool Aim::auto_pistol_enabled_ = false;
bool Aim::rapid_fire_enabled_ = false;
bool Aim::no_recoil_enabled_ = false;
bool Aim::no_spread_enabled_ = false;
bool Aim::no_scope_inaccuracy_enabled_ = false;
bool Aim::visible_only_esp_enabled_ = false;
bool Aim::show_weapon_info_enabled_ = true;
bool Aim::show_distance_enabled_ = true;
bool Aim::bomb_overlay_enabled_ = true;
bool Aim::hostage_overlay_enabled_ = true;
bool Aim::auto_strafe_enabled_ = false;
bool Aim::panic_key_enabled_ = true;
bool Aim::recoil_compensation_enabled_ = false;

float Aim::aim_x_ = 4.0f;
float Aim::aim_y_ = 4.0f;
float Aim::fov_ = 8.0f;
int Aim::aim_key_ = VK_XBUTTON2;

std::uint32_t Aim::last_trigger_time_ = 0;
std::uint32_t Aim::last_jump_time_ = 0;
std::uint32_t Aim::last_fire_time_ = 0;
std::uint32_t Aim::last_strafe_time_ = 0;

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

static float ApproachAngle(float current, float target, float smooth) {
    if (smooth <= 1.0f) {
        return target;
    }

    const float delta = NormalizeYaw(target - current);
    return current + (delta / smooth);
}

static std::uint32_t GetTickCountMs() {
    return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}

static void StopMovement() {
    const int keys[] = { 'W', 'A', 'S', 'D' };
    for (int key : keys) {
        keybd_event(static_cast<BYTE>(key), 0, KEYEVENTF_KEYUP, 0);
    }
}

static bool IsWeaponScoped(std::uintptr_t local_player_ptr) {
    if (local_player_ptr == 0) {
        return false;
    }
    if (!Memory::IsReadable(local_player_ptr + Offsets::m_bIsScoped, sizeof(bool))) {
        return false;
    }
    return Memory::Read<bool>(local_player_ptr + Offsets::m_bIsScoped);
}

static Vec3 GetBoneOffset(Aim::Bone bone) {
    switch (bone) {
    case Aim::Bone::Head:
        return {0.0f, 0.0f, 64.0f};
    case Aim::Bone::Neck:
        return {0.0f, 0.0f, 56.0f};
    case Aim::Bone::Chest:
        return {0.0f, 0.0f, 45.0f};
    case Aim::Bone::Body:
        return {0.0f, 0.0f, 50.0f};
    case Aim::Bone::Pelvis:
        return {0.0f, 0.0f, 20.0f};
    default:
        return {0.0f, 0.0f, 64.0f};
    }
}

static void DoBunnyhop() {
    const std::uint32_t now = GetTickCountMs();
    if (now - Aim::last_jump_time_ < 140) {
        return;
    }

    Aim::last_jump_time_ = now;
    keybd_event(VK_SPACE, 0, 0, 0);
    keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
}

void Aim::Update() {
    if (!master_enabled_ || !Game::IsReady()) {
        return;
    }

    const Vec3 eye = Game::GetLocalEyePosition();
    ViewAngles current = Game::GetViewAngles();
    const bool wants_aim = enabled_ && Input::IsKeyDown(aim_key_);
    const bool rage_active = ragebot_enabled_;
    const bool wallbang_active = wallbang_enabled_;
    const bool auto_shoot_active = triggerbot_enabled_ || wallbang_active || rage_active;
    const bool bunnyhop_active = bunnyhop_enabled_ || rage_active;

    const auto target_index = Game::GetBestTargetIndex(fov_);
    if (target_index.has_value()) {
        const std::uintptr_t entity = Game::GetEntity(*target_index);
        const auto origin = Game::GetEntityPosition(entity);
        if (origin.has_value()) {
            const Vec3 target_point = *origin + GetBoneOffset(aim_bone_);
            const Vec3 delta = target_point - eye;
            ViewAngles aim_angles = DirectionToAngles(delta);

            if (recoil_compensation_enabled_) {
                const Vec3 punch = Game::GetLocalViewPunch();
                aim_angles.pitch -= punch.x;
                aim_angles.yaw -= punch.y;
            }

            if (rage_active) {
                current = aim_angles;
            } else if (wants_aim) {
                const float smooth = legit_mode_enabled_ ? std::max(aim_x_, aim_y_) : 1.0f;
                current.yaw = ApproachAngle(current.yaw, aim_angles.yaw, smooth);
                current.pitch = ApproachAngle(current.pitch, aim_angles.pitch, smooth);
            }

            if (spinbot_enabled_) {
                current.yaw = NormalizeYaw(current.yaw + spinbot_speed_);
            }

            const float yaw_diff = std::abs(NormalizeYaw(aim_angles.yaw - current.yaw));
            const float pitch_diff = std::abs(aim_angles.pitch - current.pitch);
            const float target_fov = std::sqrt(yaw_diff * yaw_diff + pitch_diff * pitch_diff);

            if (auto_stop_enabled_ && target_fov <= triggerbot_fov_) {
                StopMovement();
            }

            if (auto_scope_enabled_ && !IsWeaponScoped(Game::GetLocalPlayerPtr())) {
                keybd_event(VK_RBUTTON, 0, 0, 0);
            } else {
                keybd_event(VK_RBUTTON, 0, KEYEVENTF_KEYUP, 0);
            }

            if (auto_shoot_active && target_fov <= triggerbot_fov_) {
                const std::uint32_t now = GetTickCountMs();
                const std::uint32_t fire_interval = auto_pistol_enabled_ || rapid_fire_enabled_ ? 80 : k_trigger_min_interval_ms;
                if (now - last_trigger_time_ >= fire_interval) {
                    last_trigger_time_ = now;
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
            }

            if (no_recoil_enabled_ || no_spread_enabled_ || no_scope_inaccuracy_enabled_) {
                const std::uintptr_t weapon = Game::GetActiveWeaponPtr();
                if (weapon != 0) {
                    if (no_recoil_enabled_) {
                        if (Memory::IsReadable(weapon + Offsets::m_iRecoilIndex, sizeof(int))) {
                            Memory::Write<int>(weapon + Offsets::m_iRecoilIndex, 0);
                        }
                        if (Memory::IsReadable(weapon + Offsets::m_flRecoilIndex, sizeof(float))) {
                            Memory::Write<float>(weapon + Offsets::m_flRecoilIndex, 0.0f);
                        }
                    }
                    if (no_spread_enabled_ || no_scope_inaccuracy_enabled_) {
                        const bool should_reset_inaccuracy = no_spread_enabled_ || (no_scope_inaccuracy_enabled_ && !IsWeaponScoped(Game::GetLocalPlayerPtr()));
                        if (should_reset_inaccuracy) {
                            if (Memory::IsReadable(weapon + Offsets::m_fAccuracyPenalty, sizeof(float))) {
                                Memory::Write<float>(weapon + Offsets::m_fAccuracyPenalty, 0.0f);
                            }
                            if (Memory::IsReadable(weapon + Offsets::m_flTurningInaccuracy, sizeof(float))) {
                                Memory::Write<float>(weapon + Offsets::m_flTurningInaccuracy, 0.0f);
                            }
                            if (Memory::IsReadable(weapon + Offsets::m_flTurningInaccuracyDelta, sizeof(float))) {
                                Memory::Write<float>(weapon + Offsets::m_flTurningInaccuracyDelta, 0.0f);
                            }
                        }
                    }
                }
            }
        }
    } else if (rage_active || spinbot_enabled_) {
        current.yaw = NormalizeYaw(current.yaw + spinbot_speed_);
    }

    if (bunnyhop_active) {
        DoBunnyhop();
    }

    if (auto_strafe_enabled_ && !Game::IsOnGround()) {
        const std::uint32_t now = GetTickCountMs();
        if (now - last_strafe_time_ > 90) {
            last_strafe_time_ = now;
            static bool strafe_left = false;
            const BYTE key = strafe_left ? 'A' : 'D';
            keybd_event(key, 0, 0, 0);
            keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
            strafe_left = !strafe_left;
        }
    }

    if (panic_key_enabled_ && (GetAsyncKeyState(VK_END) & 1)) {
        master_enabled_ = false;
        enabled_ = false;
        wallbang_enabled_ = false;
        bunnyhop_enabled_ = false;
        esp_enabled_ = false;
        spinbot_enabled_ = false;
        triggerbot_enabled_ = false;
        ragebot_enabled_ = false;
        auto_scope_enabled_ = false;
        auto_pistol_enabled_ = false;
        rapid_fire_enabled_ = false;
        no_recoil_enabled_ = false;
        no_spread_enabled_ = false;
        auto_strafe_enabled_ = false;
    }

    if (current.pitch > 89.0f) {
        current.pitch = 89.0f;
    }
    if (current.pitch < -89.0f) {
        current.pitch = -89.0f;
    }

    Game::SetViewAngles(current);
}

void Aim::RenderOverlay() {
    if (!Game::IsReady()) {
        return;
    }

    const bool show_esp = esp_enabled_;
    const bool show_visible_only = visible_only_esp_enabled_;
    const bool show_weapon_info = show_weapon_info_enabled_;
    const bool show_distance = show_distance_enabled_;
    const bool show_bomb_overlay = bomb_overlay_enabled_;
    const bool show_hostage_overlay = hostage_overlay_enabled_;
    if (!show_esp && !show_bomb_overlay && !show_hostage_overlay) {
        return;
    }

    const Vec3 eye = Game::GetLocalEyePosition();
    const ViewAngles view = Game::GetViewAngles();
    const int local_team = Game::GetLocalTeam();

    auto* draw_list = ImGui::GetWindowDrawList();
    const ImU32 info_color = IM_COL32(200, 200, 255, 220);
    const ImU32 warn_color = IM_COL32(255, 160, 80, 220);
    const ImU32 green_color = IM_COL32(128, 255, 128, 220);

    if (show_bomb_overlay) {
        const bool bomb_active = Game::IsBombTicking();
        const float remaining = Game::GetBombTimeRemaining();
        const std::string bomb_text = bomb_active ?
            "Bomb planted: " + std::to_string(static_cast<int>(remaining)) + "s" :
            "Bomb planted: no";
        draw_list->AddText(ImVec2(18.0f, 18.0f), bomb_active ? warn_color : info_color, bomb_text.c_str());
    }

    if (show_hostage_overlay) {
        const bool carrying = Game::IsHostageCarried();
        const std::string hostage_text = carrying ? "Hostage carried: yes" : "Hostage carried: no";
        draw_list->AddText(ImVec2(18.0f, 38.0f), carrying ? green_color : info_color, hostage_text.c_str());
    }

    for (int i = 1; i < 64; ++i) {
        const std::uintptr_t entity = Game::GetEntity(i);
        if (entity == 0 || entity == Game::GetLocalPlayerPtr()) {
            continue;
        }

        if (!show_esp || !Game::IsValidTarget(entity, local_team)) {
            continue;
        }
        if (show_visible_only && !Game::IsEntitySpotted(entity)) {
            continue;
        }

        const auto origin = Game::GetEntityPosition(entity);
        if (!origin.has_value()) {
            continue;
        }

        const Vec3 foot_position = *origin;
        const Vec3 head_position = *origin + Vec3{0.0f, 0.0f, 64.0f};

        ImVec2 screen_foot;
        ImVec2 screen_head;
        if (!Game::WorldToScreen(foot_position, screen_foot) || !Game::WorldToScreen(head_position, screen_head)) {
            continue;
        }

        const float height = std::fabs(screen_head.y - screen_foot.y);
        const float width = height * 0.45f;
        const ImVec2 top_left{screen_head.x - width * 0.5f, screen_head.y};
        const ImVec2 bottom_right{screen_head.x + width * 0.5f, screen_foot.y};

        const ImU32 box_color = IM_COL32(255, 64, 64, 220);
        const ImU32 info_color = IM_COL32(255, 255, 255, 220);
        const ImU32 secondary_color = IM_COL32(200, 200, 200, 200);

        ImGui::GetWindowDrawList()->AddRect(top_left, bottom_right, box_color, 0.0f, 0, 2.0f);
        ImGui::GetWindowDrawList()->AddLine(screen_head, screen_foot, box_color, 1.5f);
        ImGui::GetWindowDrawList()->AddText(ImVec2(top_left.x, top_left.y - 18.0f), info_color, "Enemy");
        const std::string health_text = "HP: " + std::to_string(Game::GetEntityHealth(entity));
        ImGui::GetWindowDrawList()->AddText(ImVec2(top_left.x, top_left.y - 34.0f), IM_COL32(255, 255, 64, 200), health_text.c_str());

        if (show_distance) {
            const float distance = eye.Distance(*origin);
            const std::string distance_text = "D: " + std::to_string(static_cast<int>(distance));
            ImGui::GetWindowDrawList()->AddText(ImVec2(top_left.x, top_left.y - 50.0f), secondary_color, distance_text.c_str());
        }

        if (show_weapon_info) {
            const std::uint32_t weapon_hash = Game::GetEntityWeaponHash(entity);
            const std::string weapon_text = "W: 0x" + std::to_string(weapon_hash);
            ImGui::GetWindowDrawList()->AddText(ImVec2(top_left.x, top_left.y - 66.0f), secondary_color, weapon_text.c_str());
        }

        const int armor = Game::GetEntityArmor(entity);
        const std::string armor_text = "AR: " + std::to_string(armor);
        ImGui::GetWindowDrawList()->AddText(ImVec2(top_left.x, top_left.y - 82.0f), secondary_color, armor_text.c_str());
    }
}

bool Aim::IsEnabled() { return enabled_; }
void Aim::SetEnabled(bool enabled) { enabled_ = enabled; }

bool Aim::IsMasterEnabled() { return master_enabled_; }
void Aim::SetMasterEnabled(bool enabled) { master_enabled_ = enabled; }

bool Aim::IsWallbangEnabled() { return wallbang_enabled_; }
void Aim::SetWallbangEnabled(bool enabled) { wallbang_enabled_ = enabled; }

bool Aim::IsBunnyhopEnabled() { return bunnyhop_enabled_; }
void Aim::SetBunnyhopEnabled(bool enabled) { bunnyhop_enabled_ = enabled; }

bool Aim::IsEspEnabled() { return esp_enabled_; }
void Aim::SetEspEnabled(bool enabled) { esp_enabled_ = enabled; }

bool Aim::IsSpinbotEnabled() { return spinbot_enabled_; }
void Aim::SetSpinbotEnabled(bool enabled) { spinbot_enabled_ = enabled; }

float Aim::GetSpinbotSpeed() { return spinbot_speed_; }
void Aim::SetSpinbotSpeed(float speed) { spinbot_speed_ = speed; }

bool Aim::IsTriggerbotEnabled() { return triggerbot_enabled_; }
void Aim::SetTriggerbotEnabled(bool enabled) { triggerbot_enabled_ = enabled; }

float Aim::GetTriggerbotFov() { return triggerbot_fov_; }
void Aim::SetTriggerbotFov(float fov) { triggerbot_fov_ = fov; }

bool Aim::IsRagebotEnabled() { return ragebot_enabled_; }
void Aim::SetRagebotEnabled(bool enabled) { ragebot_enabled_ = enabled; }

bool Aim::IsLegitModeEnabled() { return legit_mode_enabled_; }
void Aim::SetLegitModeEnabled(bool enabled) { legit_mode_enabled_ = enabled; }

Aim::Bone Aim::GetAimBone() { return aim_bone_; }
void Aim::SetAimBone(Bone bone) { aim_bone_ = bone; }

bool Aim::IsAutoStopEnabled() { return auto_stop_enabled_; }
void Aim::SetAutoStopEnabled(bool enabled) { auto_stop_enabled_ = enabled; }

bool Aim::IsAutoScopeEnabled() { return auto_scope_enabled_; }
void Aim::SetAutoScopeEnabled(bool enabled) { auto_scope_enabled_ = enabled; }

bool Aim::IsAutoPistolEnabled() { return auto_pistol_enabled_; }
void Aim::SetAutoPistolEnabled(bool enabled) { auto_pistol_enabled_ = enabled; }

bool Aim::IsRapidFireEnabled() { return rapid_fire_enabled_; }
void Aim::SetRapidFireEnabled(bool enabled) { rapid_fire_enabled_ = enabled; }

bool Aim::IsNoRecoilEnabled() { return no_recoil_enabled_; }
void Aim::SetNoRecoilEnabled(bool enabled) { no_recoil_enabled_ = enabled; }

bool Aim::IsNoSpreadEnabled() { return no_spread_enabled_; }
void Aim::SetNoSpreadEnabled(bool enabled) { no_spread_enabled_ = enabled; }

bool Aim::IsNoScopeInaccuracyEnabled() { return no_scope_inaccuracy_enabled_; }
void Aim::SetNoScopeInaccuracyEnabled(bool enabled) { no_scope_inaccuracy_enabled_ = enabled; }

bool Aim::IsVisibleOnlyEspEnabled() { return visible_only_esp_enabled_; }
void Aim::SetVisibleOnlyEspEnabled(bool enabled) { visible_only_esp_enabled_ = enabled; }

bool Aim::IsShowWeaponInfoEnabled() { return show_weapon_info_enabled_; }
void Aim::SetShowWeaponInfoEnabled(bool enabled) { show_weapon_info_enabled_ = enabled; }

bool Aim::IsShowDistanceEnabled() { return show_distance_enabled_; }
void Aim::SetShowDistanceEnabled(bool enabled) { show_distance_enabled_ = enabled; }

bool Aim::IsBombOverlayEnabled() { return bomb_overlay_enabled_; }
void Aim::SetBombOverlayEnabled(bool enabled) { bomb_overlay_enabled_ = enabled; }

bool Aim::IsHostageOverlayEnabled() { return hostage_overlay_enabled_; }
void Aim::SetHostageOverlayEnabled(bool enabled) { hostage_overlay_enabled_ = enabled; }

bool Aim::IsAutoStrafeEnabled() { return auto_strafe_enabled_; }
void Aim::SetAutoStrafeEnabled(bool enabled) { auto_strafe_enabled_ = enabled; }

bool Aim::IsPanicKeyEnabled() { return panic_key_enabled_; }
void Aim::SetPanicKeyEnabled(bool enabled) { panic_key_enabled_ = enabled; }

bool Aim::IsRecoilCompensationEnabled() { return recoil_compensation_enabled_; }
void Aim::SetRecoilCompensationEnabled(bool enabled) { recoil_compensation_enabled_ = enabled; }

float Aim::GetAimX() { return aim_x_; }
float Aim::GetAimY() { return aim_y_; }
void Aim::SetAimX(float value) { aim_x_ = value; }
void Aim::SetAimY(float value) { aim_y_ = value; }

float Aim::GetFov() { return fov_; }
void Aim::SetFov(float fov) { fov_ = fov; }

int Aim::GetAimKey() { return aim_key_; }
void Aim::SetAimKey(int virtual_key) { aim_key_ = virtual_key; }
