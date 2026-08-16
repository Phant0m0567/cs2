#include "Aim.hpp"

#include "Game.hpp"
#include "Input.hpp"
#include "Memory.hpp"
#include "Vec3.hpp"

#include <imgui.h>
#include <cmath>
#include <cstdint>
#include <chrono>

namespace {
constexpr float k_pi = 3.14159265358979323846f;
constexpr std::uint32_t k_trigger_min_interval_ms = 80;
}

bool Aim::enabled_ = false;
bool Aim::esp_enabled_ = true;
bool Aim::spinbot_enabled_ = false;
float Aim::spinbot_speed_ = 6.0f;
bool Aim::triggerbot_enabled_ = false;
float Aim::triggerbot_fov_ = 3.0f;
bool Aim::ragebot_enabled_ = false;

float Aim::aim_x_ = 4.0f;
float Aim::aim_y_ = 4.0f;
float Aim::fov_ = 8.0f;
int Aim::aim_key_ = VK_XBUTTON2;

std::uint32_t Aim::last_trigger_time_ = 0;

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

void Aim::Update() {
    if (!Game::IsReady()) {
        return;
    }

    const Vec3 eye = Game::GetLocalEyePosition();
    ViewAngles current = Game::GetViewAngles();
    const bool wants_aim = enabled_ && Input::IsKeyDown(aim_key_);

    const auto target = Game::GetBestTarget(fov_);
    if (target.has_value()) {
        const Vec3 delta = *target - eye;
        const ViewAngles desired = DirectionToAngles(delta);

        if (ragebot_enabled_) {
            current = desired;
        } else if (wants_aim) {
            current.yaw = ApproachAngle(current.yaw, desired.yaw, aim_x_);
            current.pitch = ApproachAngle(current.pitch, desired.pitch, aim_y_);
        }

        if (triggerbot_enabled_) {
            const float yaw_diff = std::abs(NormalizeYaw(desired.yaw - current.yaw));
            const float pitch_diff = std::abs(desired.pitch - current.pitch);
            const float target_fov = std::sqrt(yaw_diff * yaw_diff + pitch_diff * pitch_diff);
            if (target_fov <= triggerbot_fov_) {
                const std::uint32_t now = GetTickCountMs();
                if (now - last_trigger_time_ >= k_trigger_min_interval_ms) {
                    last_trigger_time_ = now;
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
            }
        }
    }

    if (spinbot_enabled_) {
        current.yaw = NormalizeYaw(current.yaw + spinbot_speed_);
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
    if (!esp_enabled_ || !Game::IsReady()) {
        return;
    }

    const Vec3 eye = Game::GetLocalEyePosition();
    const ViewAngles view = Game::GetViewAngles();
    const int local_team = Game::GetLocalTeam();

    for (int i = 1; i < 64; ++i) {
        const std::uintptr_t entity = Game::GetEntity(i);
        if (entity == 0 || entity == Game::GetLocalPlayerPtr()) {
            continue;
        }

        if (!Game::IsValidTarget(entity, local_team)) {
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

        ImGui::GetWindowDrawList()->AddRect(top_left, bottom_right, IM_COL32(255, 0, 0, 255), 2.0f, 0, 1.5f);
        ImGui::GetWindowDrawList()->AddText(ImVec2(top_left.x, top_left.y - 14.0f), IM_COL32(255, 255, 255, 255), "Enemy");
    }
}

bool Aim::IsEnabled() { return enabled_; }
void Aim::SetEnabled(bool enabled) { enabled_ = enabled; }

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

float Aim::GetAimX() { return aim_x_; }
float Aim::GetAimY() { return aim_y_; }
void Aim::SetAimX(float value) { aim_x_ = value; }
void Aim::SetAimY(float value) { aim_y_ = value; }

float Aim::GetFov() { return fov_; }
void Aim::SetFov(float fov) { fov_ = fov; }

int Aim::GetAimKey() { return aim_key_; }
void Aim::SetAimKey(int virtual_key) { aim_key_ = virtual_key; }
