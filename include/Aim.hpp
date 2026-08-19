#pragma once

#include <cstdint>

class Aim {
public:
    enum class Bone {
        Head = 0,
        Neck,
        Chest,
        Pelvis,
        Body,
    };

    static void Update();
    static void RenderOverlay();

    static bool IsEnabled();
    static void SetEnabled(bool enabled);
    static bool IsMasterEnabled();
    static void SetMasterEnabled(bool enabled);
    static bool IsWallbangEnabled();
    static void SetWallbangEnabled(bool enabled);
    static bool IsBunnyhopEnabled();
    static void SetBunnyhopEnabled(bool enabled);
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
    static bool IsLegitModeEnabled();
    static void SetLegitModeEnabled(bool enabled);
    static Bone GetAimBone();
    static void SetAimBone(Bone bone);
    static bool IsAutoStopEnabled();
    static void SetAutoStopEnabled(bool enabled);
    static bool IsAutoStopWhenShootingEnabled();
    static void SetAutoStopWhenShootingEnabled(bool enabled);
    static bool IsAutoScopeEnabled();
    static void SetAutoScopeEnabled(bool enabled);
    static bool IsAutoPistolEnabled();
    static void SetAutoPistolEnabled(bool enabled);
    static bool IsRapidFireEnabled();
    static void SetRapidFireEnabled(bool enabled);
    static int GetTriggerDelay();
    static void SetTriggerDelay(int delay_ms);
    static bool IsNoRecoilEnabled();
    static void SetNoRecoilEnabled(bool enabled);
    static bool IsNoSpreadEnabled();
    static void SetNoSpreadEnabled(bool enabled);
    static bool IsNoScopeInaccuracyEnabled();
    static void SetNoScopeInaccuracyEnabled(bool enabled);
    static bool IsPanicKeyEnabled();
    static void SetPanicKeyEnabled(bool enabled);
    static int GetPanicKey();
    static void SetPanicKey(int virtual_key);
    static void ResetDefaults();

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
    static bool master_enabled_;
    static bool wallbang_enabled_;
    static bool bunnyhop_enabled_;
    static bool esp_enabled_;
    static bool spinbot_enabled_;
    static float spinbot_speed_;
    static bool triggerbot_enabled_;
    static float triggerbot_fov_;
    static bool ragebot_enabled_;
    static bool legit_mode_enabled_;
    static Bone aim_bone_;
    static bool auto_stop_enabled_;
    static bool auto_scope_enabled_;
    static bool auto_pistol_enabled_;
    static bool rapid_fire_enabled_;
    static bool no_recoil_enabled_;
    static bool no_spread_enabled_;
    static bool no_scope_inaccuracy_enabled_;
    static bool auto_strafe_enabled_;
    static bool panic_key_enabled_;

    static float aim_x_;
    static float aim_y_;
    static float fov_;
    static int aim_key_;
    static int panic_key_;
    static int trigger_delay_ms_;
    static bool auto_stop_when_shooting_;
    static bool recoil_compensation_enabled_;

    static std::uint32_t last_trigger_time_;
    static std::uint32_t last_jump_time_;
    static std::uint32_t last_fire_time_;
    static std::uint32_t last_strafe_time_;
};
