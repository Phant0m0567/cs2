#pragma once

#include <cstdint>

class Aim {
public:
    static void Update();
    static void RenderOverlay();

    static bool IsEnabled();
    static void SetEnabled(bool enabled);
    static bool IsEspEnabled();
    static void SetEspEnabled(bool enabled);
    static bool IsSpinbotEnabled();
    static void SetSpinbotEnabled(bool enabled);
    static float GetSpinbotSpeed();
    static void SetSpinbotSpeed(float speed);
    static bool IsTriggerbotEnabled();
    static void SetTriggerbotEnabled(bool enabled);
    static float GetTriggerbotFov();
    static void SetTriggerbotFov(float fov);
    static bool IsRagebotEnabled();
    static void SetRagebotEnabled(bool enabled);

    static float GetAimX();
    static float GetAimY();
    static void SetAimX(float value);
    static void SetAimY(float value);

    static float GetFov();
    static void SetFov(float fov);

    static int GetAimKey();
    static void SetAimKey(int virtual_key);

private:
    static bool enabled_;
    static bool esp_enabled_;
    static bool spinbot_enabled_;
    static float spinbot_speed_;
    static bool triggerbot_enabled_;
    static float triggerbot_fov_;
    static bool ragebot_enabled_;

    static float aim_x_;
    static float aim_y_;
    static float fov_;
    static int aim_key_;

    static std::uint32_t last_trigger_time_; 
};
