#include "Config.hpp"

#include "Aim.hpp"
#include "Glow.hpp"

#include <fstream>
#include <string>

namespace {
    const char* kConfigFileName = "cs2_math.cfg";

    std::string GetProfileFileName(const std::string& profile_name) {
        if (profile_name.empty() || profile_name == "default") {
            return std::string(kConfigFileName);
        }
        return "cs2_math_" + profile_name + ".cfg";
    }

    bool ParseBool(const std::string& value) {
        return value == "1" || value == "true";
    }
}

std::string Config::current_profile_ = "default";

bool Config::LoadFromFile(const std::string& file_name) {
    std::ifstream config(file_name);
    if (!config.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(config, line)) {
        if (line.rfind("profile=", 0) == 0) {
            current_profile_ = line.substr(8);
        } else if (line.rfind("master=", 0) == 0) {
            Aim::SetMasterEnabled(ParseBool(line.substr(7)));
        } else if (line.rfind("aim=", 0) == 0) {
            Aim::SetEnabled(ParseBool(line.substr(4)));
        } else if (line.rfind("wallbang=", 0) == 0) {
            Aim::SetWallbangEnabled(ParseBool(line.substr(9)));
        } else if (line.rfind("bunnyhop=", 0) == 0) {
            Aim::SetBunnyhopEnabled(ParseBool(line.substr(9)));
        } else if (line.rfind("esp=", 0) == 0) {
            Aim::SetEspEnabled(ParseBool(line.substr(4)));
        } else if (line.rfind("spinbot=", 0) == 0) {
            Aim::SetSpinbotEnabled(ParseBool(line.substr(8)));
        } else if (line.rfind("spinbot_speed=", 0) == 0) {
            Aim::SetSpinbotSpeed(std::stof(line.substr(14)));
        } else if (line.rfind("triggerbot=", 0) == 0) {
            Aim::SetTriggerbotEnabled(ParseBool(line.substr(11)));
        } else if (line.rfind("triggerbot_fov=", 0) == 0) {
            Aim::SetTriggerbotFov(std::stof(line.substr(14)));
        } else if (line.rfind("ragebot=", 0) == 0) {
            Aim::SetRagebotEnabled(ParseBool(line.substr(8)));
        } else if (line.rfind("legit=", 0) == 0) {
            Aim::SetLegitModeEnabled(ParseBool(line.substr(6)));
        } else if (line.rfind("aim_bone=", 0) == 0) {
            const int bone = std::stoi(line.substr(9));
            if (bone >= 0 && bone <= 4) {
                Aim::SetAimBone(static_cast<Aim::Bone>(bone));
            }
        } else if (line.rfind("auto_stop=", 0) == 0) {
            Aim::SetAutoStopEnabled(ParseBool(line.substr(10)));
        } else if (line.rfind("auto_scope=", 0) == 0) {
            Aim::SetAutoScopeEnabled(ParseBool(line.substr(11)));
        } else if (line.rfind("auto_pistol=", 0) == 0) {
            Aim::SetAutoPistolEnabled(ParseBool(line.substr(12)));
        } else if (line.rfind("rapid_fire=", 0) == 0) {
            Aim::SetRapidFireEnabled(ParseBool(line.substr(11)));
        } else if (line.rfind("no_recoil=", 0) == 0) {
            Aim::SetNoRecoilEnabled(ParseBool(line.substr(10)));
        } else if (line.rfind("no_spread=", 0) == 0) {
            Aim::SetNoSpreadEnabled(ParseBool(line.substr(11)));
        } else if (line.rfind("no_scope_inaccuracy=", 0) == 0) {
            Aim::SetNoScopeInaccuracyEnabled(ParseBool(line.substr(20)));
        } else if (line.rfind("visible_only_esp=", 0) == 0) {
            Aim::SetVisibleOnlyEspEnabled(ParseBool(line.substr(17)));
        } else if (line.rfind("show_weapon_info=", 0) == 0) {
            Aim::SetShowWeaponInfoEnabled(ParseBool(line.substr(17)));
        } else if (line.rfind("show_distance=", 0) == 0) {
            Aim::SetShowDistanceEnabled(ParseBool(line.substr(14)));
        } else if (line.rfind("bomb_overlay=", 0) == 0) {
            Aim::SetBombOverlayEnabled(ParseBool(line.substr(13)));
        } else if (line.rfind("hostage_overlay=", 0) == 0) {
            Aim::SetHostageOverlayEnabled(ParseBool(line.substr(15)));
        } else if (line.rfind("auto_strafe=", 0) == 0) {
            Aim::SetAutoStrafeEnabled(ParseBool(line.substr(12)));
        } else if (line.rfind("panic_key=", 0) == 0) {
            Aim::SetPanicKeyEnabled(ParseBool(line.substr(10)));
        } else if (line.rfind("recoil=", 0) == 0) {
            Aim::SetRecoilCompensationEnabled(ParseBool(line.substr(7)));
        } else if (line.rfind("glow=", 0) == 0) {
            Glow::SetEnabled(ParseBool(line.substr(5)));
        } else if (line.rfind("hostage_glow=", 0) == 0) {
            Glow::SetHostageGlowEnabled(ParseBool(line.substr(12)));
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
            Glow::SetThroughWalls(ParseBool(line.substr(18)));
        }
    }

    return true;
}

void Config::SaveToFile(const std::string& file_name) {
    std::ofstream config(file_name, std::ios::trunc);
    if (!config.is_open()) {
        return;
    }

    config << "profile=" << current_profile_ << '\n';
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
    config << "visible_only_esp=" << (Aim::IsVisibleOnlyEspEnabled() ? 1 : 0) << '\n';
    config << "show_weapon_info=" << (Aim::IsShowWeaponInfoEnabled() ? 1 : 0) << '\n';
    config << "show_distance=" << (Aim::IsShowDistanceEnabled() ? 1 : 0) << '\n';
    config << "bomb_overlay=" << (Aim::IsBombOverlayEnabled() ? 1 : 0) << '\n';
    config << "hostage_overlay=" << (Aim::IsHostageOverlayEnabled() ? 1 : 0) << '\n';
    config << "auto_strafe=" << (Aim::IsAutoStrafeEnabled() ? 1 : 0) << '\n';
    config << "panic_key=" << (Aim::IsPanicKeyEnabled() ? 1 : 0) << '\n';
    config << "recoil=" << (Aim::IsRecoilCompensationEnabled() ? 1 : 0) << '\n';
    config << "glow=" << (Glow::IsEnabled() ? 1 : 0) << '\n';
    config << "hostage_glow=" << (Glow::IsHostageGlowEnabled() ? 1 : 0) << '\n';
    config << "glow_color=" << Glow::GetRed() << ',' << Glow::GetGreen() << ',' << Glow::GetBlue() << ',' << Glow::GetAlpha() << '\n';
    config << "glow_through_walls=" << (Glow::IsThroughWalls() ? 1 : 0) << '\n';
}

void Config::Load() {
    current_profile_ = "default";
    LoadFromFile(kConfigFileName);
}

void Config::Save() {
    SaveToFile(GetProfileFileName(current_profile_));
}

void Config::LoadProfile(const std::string& profile_name) {
    const std::string file_name = GetProfileFileName(profile_name);
    if (LoadFromFile(file_name)) {
        current_profile_ = profile_name.empty() ? "default" : profile_name;
    }
}

void Config::SaveProfile(const std::string& profile_name) {
    current_profile_ = profile_name.empty() ? "default" : profile_name;
    const std::string file_name = GetProfileFileName(current_profile_);
    SaveToFile(file_name);
}

const std::string& Config::GetCurrentProfile() {
    return current_profile_;
}
