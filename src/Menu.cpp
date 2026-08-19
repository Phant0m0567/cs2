#include "Menu.hpp"

#include "Aim.hpp"
#include "Config.hpp"
#include "Glow.hpp"
#include "Game.hpp"

#include <imgui.h>

#include <string>
#include <vector>

bool Menu::visible_ = false;

static std::vector<std::string> log_lines;

void Menu::Setup() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.11f, 0.96f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.35f, 0.65f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.35f, 0.70f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.70f, 1.0f, 1.0f);
}

void Menu::RenderAimTab() {
    bool master = Aim::IsMasterEnabled();
    if (ImGui::Checkbox("Master switch", &master)) {
        Aim::SetMasterEnabled(master);
    }

    bool enabled = Aim::IsEnabled();
    if (ImGui::Checkbox("Enable aim", &enabled)) {
        Aim::SetEnabled(enabled);
    }

    bool wallbang = Aim::IsWallbangEnabled();
    if (ImGui::Checkbox("Enable wallbang", &wallbang)) {
        Aim::SetWallbangEnabled(wallbang);
    }

    bool bunnyhop = Aim::IsBunnyhopEnabled();
    if (ImGui::Checkbox("Enable bunnyhop", &bunnyhop)) {
        Aim::SetBunnyhopEnabled(bunnyhop);
    }

    float aim_x = Aim::GetAimX();
    float aim_y = Aim::GetAimY();
    float fov = Aim::GetFov();

    ImGui::SliderFloat("Aim X (yaw smooth)", &aim_x, 1.0f, 20.0f, "%.1f");
    ImGui::SliderFloat("Aim Y (pitch smooth)", &aim_y, 1.0f, 20.0f, "%.1f");
    ImGui::SliderFloat("FOV", &fov, 1.0f, 30.0f, "%.1f deg");

    Aim::SetAimX(aim_x);
    Aim::SetAimY(aim_y);
    Aim::SetFov(fov);

    ImGui::Separator();
    ImGui::Text("Hold Mouse 5 to aim when Aim is enabled");
    ImGui::Text("Lower smooth = faster snap");

    bool esp = Aim::IsEspEnabled();
    if (ImGui::Checkbox("Enable ESP", &esp)) {
        Aim::SetEspEnabled(esp);
    }

    bool spinbot = Aim::IsSpinbotEnabled();
    if (ImGui::Checkbox("Enable Spinbot", &spinbot)) {
        Aim::SetSpinbotEnabled(spinbot);
    }

    float spin_speed = Aim::GetSpinbotSpeed();
    ImGui::SliderFloat("Spinbot speed", &spin_speed, 1.0f, 30.0f, "%.1f");
    Aim::SetSpinbotSpeed(spin_speed);

    bool triggerbot = Aim::IsTriggerbotEnabled();
    if (ImGui::Checkbox("Enable Triggerbot", &triggerbot)) {
        Aim::SetTriggerbotEnabled(triggerbot);
    }

    float trigger_fov = Aim::GetTriggerbotFov();
    ImGui::SliderFloat("Triggerbot FOV", &trigger_fov, 0.5f, 10.0f, "%.1f");
    Aim::SetTriggerbotFov(trigger_fov);

    bool ragebot = Aim::IsRagebotEnabled();
    if (ImGui::Checkbox("Enable Ragebot", &ragebot)) {
        Aim::SetRagebotEnabled(ragebot);
    }

    bool legit = Aim::IsLegitModeEnabled();
    if (ImGui::Checkbox("Enable Legit mode", &legit)) {
        Aim::SetLegitModeEnabled(legit);
    }

    const char* bone_names[] = { "Head", "Neck", "Chest", "Body", "Pelvis" };
    int bone_index = static_cast<int>(Aim::GetAimBone());
    if (ImGui::Combo("Aim bone", &bone_index, bone_names, IM_ARRAYSIZE(bone_names))) {
        Aim::SetAimBone(static_cast<Aim::Bone>(bone_index));
    }

    bool auto_stop = Aim::IsAutoStopEnabled();
    if (ImGui::Checkbox("Enable auto stop", &auto_stop)) {
        Aim::SetAutoStopEnabled(auto_stop);
    }

    bool auto_scope = Aim::IsAutoScopeEnabled();
    if (ImGui::Checkbox("Enable auto scope", &auto_scope)) {
        Aim::SetAutoScopeEnabled(auto_scope);
    }

    bool auto_pistol = Aim::IsAutoPistolEnabled();
    if (ImGui::Checkbox("Enable auto pistol", &auto_pistol)) {
        Aim::SetAutoPistolEnabled(auto_pistol);
    }

    bool rapid_fire = Aim::IsRapidFireEnabled();
    if (ImGui::Checkbox("Enable rapid fire", &rapid_fire)) {
        Aim::SetRapidFireEnabled(rapid_fire);
    }

    bool no_recoil = Aim::IsNoRecoilEnabled();
    if (ImGui::Checkbox("Enable no recoil", &no_recoil)) {
        Aim::SetNoRecoilEnabled(no_recoil);
    }

    bool no_spread = Aim::IsNoSpreadEnabled();
    if (ImGui::Checkbox("Enable no spread", &no_spread)) {
        Aim::SetNoSpreadEnabled(no_spread);
    }

    bool no_scope = Aim::IsNoScopeInaccuracyEnabled();
    if (ImGui::Checkbox("Enable no-scope inaccuracy", &no_scope)) {
        Aim::SetNoScopeInaccuracyEnabled(no_scope);
    }

    bool auto_strafe = Aim::IsAutoStrafeEnabled();
    if (ImGui::Checkbox("Enable auto strafe", &auto_strafe)) {
        Aim::SetAutoStrafeEnabled(auto_strafe);
    }

    bool panic = Aim::IsPanicKeyEnabled();
    if (ImGui::Checkbox("Enable panic key (END)", &panic)) {
        Aim::SetPanicKeyEnabled(panic);
    }

    ImGui::Separator();
    ImGui::Text("Spinbot ignores aim key and rotates yaw constantly.");
    ImGui::Text("Triggerbot fires when the best target is near the crosshair.");
    ImGui::Text("Ragebot snaps instantly to the target and spams shots.");

    ImGui::Separator();
    bool glow_enabled = Glow::IsEnabled();
    if (ImGui::Checkbox("Enable Glow", &glow_enabled)) {
        Glow::SetEnabled(glow_enabled);
    }
    float glow_color[4] = { Glow::GetRed(), Glow::GetGreen(), Glow::GetBlue(), Glow::GetAlpha() };
    if (ImGui::ColorEdit4("Glow Color", glow_color)) {
        Glow::SetColor(glow_color[0], glow_color[1], glow_color[2], glow_color[3]);
    }
    bool through_walls = Glow::IsThroughWalls();
    if (ImGui::Checkbox("Glow Through Walls", &through_walls)) {
        Glow::SetThroughWalls(through_walls);
    }

    bool hostage_glow = Glow::IsHostageGlowEnabled();
    if (ImGui::Checkbox("Enable hostage glow", &hostage_glow)) {
        Glow::SetHostageGlowEnabled(hostage_glow);
    }

    ImGui::Separator();
    if (ImGui::Button("Save config")) {
        Config::Save();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load config")) {
        Config::Load();
    }
}

void Menu::RenderStatusTab() {
    ImGui::Text("Game ready: %s", Game::IsReady() ? "yes" : "no");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::Text("Master enabled: %s", Aim::IsMasterEnabled() ? "yes" : "no");
    ImGui::Text("Aimbot enabled: %s", Aim::IsEnabled() ? "yes" : "no");
    ImGui::Text("Recoil compensation: %s", Aim::IsRecoilCompensationEnabled() ? "yes" : "no");
    ImGui::Separator();
    const auto target_index = Game::GetBestTargetIndex(Aim::GetFov());
    if (target_index.has_value()) {
        const std::uintptr_t target_entity = Game::GetEntity(*target_index);
        ImGui::Text("Best target index: %d", *target_index);
        ImGui::Text("Target health: %d", Game::GetEntityHealth(target_entity));
        ImGui::Text("Target team: %d", Game::GetEntityTeam(target_entity));
    } else {
        ImGui::Text("Best target index: none");
    }

    ImGui::Separator();
    ImGui::Text("Bomb planted: %s", Game::IsBombTicking() ? "yes" : "no");
    ImGui::Text("Bomb time remaining: %.1f", Game::GetBombTimeRemaining());
    ImGui::Text("Hostage carried: %s", Game::IsHostageCarried() ? "yes" : "no");
    ImGui::Separator();
    ImGui::TextWrapped(
        "Set offsets in include/Offsets.hpp after each CS2 update. "
        "Use pattern scans in client.dll to find entity_list, local_player, and view_angles."
    );
}

void Menu::RenderDebugTab() {
    ImGui::Text("Glow property: 0x%llx", static_cast<unsigned long long>(Glow::GetGlowPointer()));
    ImGui::Text("Glow writes: %d", Glow::GetGlowCount());
    ImGui::Separator();

    ImGui::Text("Local health: %d", Game::GetLocalHealth());
    ImGui::Text("Local team: %d", Game::GetLocalTeam());
    ImGui::Separator();

    const auto target_index = Game::GetBestTargetIndex(Aim::GetFov());
    if (target_index.has_value()) {
        const std::uintptr_t target_entity = Game::GetEntity(*target_index);
        ImGui::Text("Closest target index: %d", *target_index);
        ImGui::Text("Target health: %d", Game::GetEntityHealth(target_entity));
        ImGui::Text("Target team: %d", Game::GetEntityTeam(target_entity));
    } else {
        ImGui::Text("Closest target index: none");
    }
}

void Menu::RenderLogTab() {
    ImGui::BeginChild("LogScroll", ImVec2(0.0f, 0.0f), true);
    for (const std::string& line : log_lines) {
        ImGui::TextUnformatted(line.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}

void Menu::Render() {
    if (!visible_) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(420.0f, 360.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("cs2_math", &visible_, ImGuiWindowFlags_NoCollapse)) {
        if (ImGui::BeginTabBar("MainTabs")) {
            if (ImGui::BeginTabItem("Aim")) {
                RenderAimTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Status")) {
                RenderStatusTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Debug")) {
                RenderDebugTab();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Log")) {
                RenderLogTab();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    ImGui::End();
}

void Menu::Shutdown() {
    log_lines.clear();
}

void Menu::SetVisible(bool visible) {
    visible_ = visible;
}

bool Menu::IsVisible() {
    return visible_;
}

void Menu::Toggle() {
    visible_ = !visible_;
}

void Menu::AddLog(const char* message) {
    log_lines.emplace_back(message);
}
