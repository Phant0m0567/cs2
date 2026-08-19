#include "Config.hpp"

#include "Aim.hpp"
#include "Glow.hpp"

#include <fstream>
#include <string>

namespace {
    const char* kConfigFileName = "cs2_math.cfg";
}

void Config::Load() {
    std::ifstream config(kConfigFileName);
    if (!config.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(config, line)) {
        if (line.rfind("master=", 0) == 0) {
            Aim::SetMasterEnabled(line.substr(7) == "1");
        } else if (line.rfind("aim=", 0) == 0) {
            Aim::SetEnabled(line.substr(4) == "1");
        } else if (line.rfind("wallbang=", 0) == 0) {
            Aim::SetWallbangEnabled(line.substr(9) == "1");
        } else if (line.rfind("bunnyhop=", 0) == 0) {
            Aim::SetBunnyhopEnabled(line.substr(9) == "1");
        } else if (line.rfind("esp=", 0) == 0) {
            Aim::SetEspEnabled(line.substr(4) == "1");
        } else if (line.rfind("spinbot=", 0) == 0) {
            Aim::SetSpinbotEnabled(line.substr(8) == "1");
        } else if (line.rfind("spinbot_speed=", 0) == 0) {
            Aim::SetSpinbotSpeed(std::stof(line.substr(14)));
        } else if (line.rfind("triggerbot=", 0) == 0) {
            Aim::SetTriggerbotEnabled(line.substr(11) == "1");
        } else if (line.rfind("triggerbot_fov=", 0) == 0) {
            Aim::SetTriggerbotFov(std::stof(line.substr(14)));
        } else if (line.rfind("ragebot=", 0) == 0) {
            Aim::SetRagebotEnabled(line.substr(8) == "1");
        } else if (line.rfind("legit=", 0) == 0) {
            Aim::SetLegitModeEnabled(line.substr(6) == "1");
        } else if (line.rfind("aim_bone=", 0) == 0) {
            const int bone = std::stoi(line.substr(9));
            if (bone >= 0 && bone <= 4) {
                Aim::SetAimBone(static_cast<Aim::Bone>(bone));
            }
        } else if (line.rfind("auto_stop=", 0) == 0) {
            Aim::SetAutoStopEnabled(line.substr(10) == "1");
        } else if (line.rfind("auto_scope=", 0) == 0) {
            Aim::SetAutoScopeEnabled(line.substr(11) == "1");
        } else if (line.rfind("auto_pistol=", 0) == 0) {
            Aim::SetAutoPistolEnabled(line.substr(12) == "1");
        } else if (line.rfind("rapid_fire=", 0) == 0) {
            Aim::SetRapidFireEnabled(line.substr(11) == "1");
        } else if (line.rfind("no_recoil=", 0) == 0) {
            Aim::SetNoRecoilEnabled(line.substr(10) == "1");
        } else if (line.rfind("no_spread=", 0) == 0) {
            Aim::SetNoSpreadEnabled(line.substr(11) == "1");
        } else if (line.rfind("no_scope_inaccuracy=", 0) == 0) {
            Aim::SetNoScopeInaccuracyEnabled(line.substr(20) == "1");
        } else if (line.rfind("auto_strafe=", 0) == 0) {
            Aim::SetAutoStrafeEnabled(line.substr(12) == "1");
        } else if (line.rfind("panic_key=", 0) == 0) {
            Aim::SetPanicKeyEnabled(line.substr(10) == "1");
        } else if (line.rfind("recoil=", 0) == 0) {
            Aim::SetRecoilCompensationEnabled(line.substr(7) == "1");
        } else if (line.rfind("glow=", 0) == 0) {
            Glow::SetEnabled(line.substr(5) == "1");
        } else if (line.rfind("hostage_glow=", 0) == 0) {
            Glow::SetHostageGlowEnabled(line.substr(12) == "1");
        } else if (line.rfind("glow_color=", 0) == 0) {
            const auto values = line.substr(11);
            const size_t first = values.find(',');
            const size_t second = values.find_last_of(',');
            if (first != std::string::npos && second != std::string::npos && first != second) {
                const float r = std::stof(values.substr(0, first));
                const float g = std::stof(values.substr(first + 1, second - first - 1));
                const size_t third = values.find(',', second + 1);
                if (third == std::string::npos) {
                    const float b = std::stof(values.substr(second + 1));
                    Glow::SetColor(r, g, b, Glow::GetAlpha());
                } else {
                    const float b = std::stof(values.substr(second + 1, third - second - 1));
                    const float a = std::stof(values.substr(third + 1));
                    Glow::SetColor(r, g, b, a);
                }
            }
        } else if (line.rfind("glow_through_walls=", 0) == 0) {
            Glow::SetThroughWalls(line.substr(18) == "1");
        }
    }
}

void Config::Save() {
    std::ofstream config(kConfigFileName, std::ios::trunc);
    if (!config.is_open()) {
        return;
    }

    config << "master=" << (Aim::IsMasterEnabled() ? 1 : 0) << '\n';
    config << "aim=" << (Aim::IsEnabled() ? 1 : 0) << '\n';
    config << "wallbang=" << (Aim::IsWallbangEnabled() ? 1 : 0) << '\n';
    config << "bunnyhop=" << (Aim::IsBunnyhopEnabled() ? 1 : 0) << '\n';
    config << "esp=" << (Aim::IsEspEnabled() ? 1 : 0) << '\n';
    config << "spinbot=" << (Aim::IsSpinbotEnabled() ? 1 : 0) << '\n';
    config << "spinbot_speed=" << Aim::GetSpinbotSpeed() << '\n';
    config << "triggerbot=" << (Aim::IsTriggerbotEnabled() ? 1 : 0) << '\n';
    config << "triggerbot_fov=" << Aim::GetTriggerbotFov() << '\n';
    config << "ragebot=" << (Aim::IsRagebotEnabled() ? 1 : 0) << '\n';
    config << "legit=" << (Aim::IsLegitModeEnabled() ? 1 : 0) << '\n';
    config << "aim_bone=" << static_cast<int>(Aim::GetAimBone()) << '\n';
    config << "auto_stop=" << (Aim::IsAutoStopEnabled() ? 1 : 0) << '\n';
    config << "auto_scope=" << (Aim::IsAutoScopeEnabled() ? 1 : 0) << '\n';
    config << "auto_pistol=" << (Aim::IsAutoPistolEnabled() ? 1 : 0) << '\n';
    config << "rapid_fire=" << (Aim::IsRapidFireEnabled() ? 1 : 0) << '\n';
    config << "no_recoil=" << (Aim::IsNoRecoilEnabled() ? 1 : 0) << '\n';
    config << "no_spread=" << (Aim::IsNoSpreadEnabled() ? 1 : 0) << '\n';
    config << "no_scope_inaccuracy=" << (Aim::IsNoScopeInaccuracyEnabled() ? 1 : 0) << '\n';
    config << "auto_strafe=" << (Aim::IsAutoStrafeEnabled() ? 1 : 0) << '\n';
    config << "panic_key=" << (Aim::IsPanicKeyEnabled() ? 1 : 0) << '\n';
    config << "recoil=" << (Aim::IsRecoilCompensationEnabled() ? 1 : 0) << '\n';
    config << "glow=" << (Glow::IsEnabled() ? 1 : 0) << '\n';
    config << "hostage_glow=" << (Glow::IsHostageGlowEnabled() ? 1 : 0) << '\n';
    config << "glow_color=" << Glow::GetRed() << ',' << Glow::GetGreen() << ',' << Glow::GetBlue() << ',' << Glow::GetAlpha() << '\n';
    config << "glow_through_walls=" << (Glow::IsThroughWalls() ? 1 : 0) << '\n';
}
