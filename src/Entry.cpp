#include "Entry.hpp"

#include "Aim.hpp"
#include "Game.hpp"
#include "Glow.hpp"
#include "Menu.hpp"
#include "Renderer.hpp"

HMODULE Entry::module_ = nullptr;
HANDLE Entry::thread_ = nullptr;
volatile bool Entry::should_run_ = false;

bool Entry::OnAttach(HMODULE module) {
    module_ = module;
    should_run_ = true;

    DisableThreadLibraryCalls(module_);

    thread_ = CreateThread(
        nullptr,
        0,
        ThreadProc,
        nullptr,
        0,
        nullptr
    );

    return thread_ != nullptr;
}

void Entry::OnDetach(bool process_terminating) {
    should_run_ = false;

    if (process_terminating) {
        Renderer::Shutdown();
        return;
    }

    if (thread_ != nullptr) {
        WaitForSingleObject(thread_, INFINITE);
        CloseHandle(thread_);
        thread_ = nullptr;
    }
}

DWORD WINAPI Entry::ThreadProc(LPVOID param) {
    (void)param;

    Initialize();
    RunLoop();
    Unload();

    return 0;
}

void Entry::Initialize() {
    Sleep(3000);

    bool game_initialized = false;
    for (int attempt = 0; attempt < 30 && should_run_ && !game_initialized; ++attempt) {
        game_initialized = Game::Initialize();
        if (!game_initialized) {
            Sleep(1000);
        }
    }

    if (game_initialized) {
        Menu::AddLog("client.dll loaded");
    } else {
        Menu::AddLog("client.dll not found after wait");
    }

    bool renderer_initialized = false;
    for (int attempt = 0; attempt < 30 && should_run_ && !renderer_initialized; ++attempt) {
        renderer_initialized = Renderer::Initialize();
        if (!renderer_initialized) {
            Sleep(1000);
        }
    }

    if (renderer_initialized) {
        Menu::AddLog("Renderer hooked");
    } else {
        Menu::AddLog("Renderer hook failed after wait");
    }

    if (!Game::IsReady()) {
        Menu::AddLog("Offsets not set - update Offsets.hpp");
    }
}

void Entry::Unload() {
    Renderer::Shutdown();
}

void Entry::RunLoop() {
    while (should_run_) {
        Aim::Update();
        Glow::Update();
        Sleep(1);
    }
}
