#pragma once

class Menu {
public:
    static void Setup();
    static void Render();
    static void Shutdown();

    static void SetVisible(bool visible);
    static bool IsVisible();
    static void Toggle();

    static void AddLog(const char* message);

private:
    static void RenderAimTab();
    static void RenderStatusTab();
    static void RenderLogTab();

    static bool visible_;
};
