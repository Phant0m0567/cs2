#include "Menu.hpp"

#include "Aim.hpp"
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
    bool enabled = Aim::IsEnabled();
    if (ImGui::Checkbox("Enable aim", &enabled)) {
        Aim::SetEnabled(enabled);
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
    ImGui::Text("Hold Mouse 5 to aim");
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

    ImGui::Separator();
    ImGui::Text("Spinbot ignores aim key and rotates yaw constantly.");
    ImGui::Text("Triggerbot fires when the best target is near the crosshair.");
    ImGui::Text("Ragebot snaps instantly to the target.");
}

void Menu::RenderStatusTab() {
    ImGui::Text("Game ready: %s", Game::IsReady() ? "yes" : "no");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::TextWrapped(
        "Set offsets in include/Offsets.hpp after each CS2 update. "
        "Use pattern scans in client.dll to find entity_list, local_player, and view_angles."
    );
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
