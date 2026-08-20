#include "Renderer.hpp"

#include "Aim.hpp"
#include "Menu.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <MinHook.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hwnd,
    UINT msg,
    WPARAM w_param,
    LPARAM l_param
);

Renderer::PresentFn Renderer::original_present_ = nullptr;
Renderer::ResizeBuffersFn Renderer::original_resize_buffers_ = nullptr;
Renderer::WndProcFn Renderer::original_wnd_proc_ = nullptr;

ID3D11Device* Renderer::device_ = nullptr;
ID3D11DeviceContext* Renderer::context_ = nullptr;
ID3D11RenderTargetView* Renderer::render_target_ = nullptr;
HWND Renderer::window_ = nullptr;
void* Renderer::present_target_ = nullptr;
void* Renderer::resize_target_ = nullptr;

std::atomic<bool> Renderer::imgui_ready_{false};
std::atomic<bool> Renderer::hook_ready_{false};
std::atomic<bool> Renderer::shutting_down_{false};

bool Renderer::GetSwapChainVtable(void** out_present, void** out_resize_buffers) {
    WNDCLASSEXA window_class{
        CS_CLASSDC,
        DefWindowProcA,
        0,
        0,
        GetModuleHandleA(nullptr),
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        "D3D11DummyWindow",
        nullptr
    };

    if (!RegisterClassExA(&window_class)) {
        return false;
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "D3D11DummyWindow",
        WS_OVERLAPPEDWINDOW,
        0, 0, 100, 100,
        nullptr,
        nullptr,
        window_class.hInstance,
        nullptr
    );

    if (dummy_window == nullptr) {
        UnregisterClassA(window_class.lpszClassName, window_class.hInstance);
        return false;
    }

    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = dummy_window;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* swap_chain = nullptr;
    ID3D11Device* dummy_device = nullptr;
    ID3D11DeviceContext* dummy_context = nullptr;

    const D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_0
    };
    D3D_FEATURE_LEVEL feature_level{};

    const HRESULT create_result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        feature_levels,
        2,
        D3D11_SDK_VERSION,
        &swap_chain_desc,
        &swap_chain,
        &dummy_device,
        &feature_level,
        &dummy_context
    );

    if (FAILED(create_result)) {
        DestroyWindow(dummy_window);
        UnregisterClassA(window_class.lpszClassName, window_class.hInstance);
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(swap_chain);
    *out_present = vtable[8];
    *out_resize_buffers = vtable[13];

    swap_chain->Release();
    dummy_device->Release();
    dummy_context->Release();
    DestroyWindow(dummy_window);
    UnregisterClassA(window_class.lpszClassName, window_class.hInstance);

    return true;
}

void Renderer::ReleaseRenderTarget() {
    if (render_target_ != nullptr) {
        render_target_->Release();
        render_target_ = nullptr;
    }
}

void Renderer::CreateRenderTarget(IDXGISwapChain* swap_chain) {
    ReleaseRenderTarget();

    ID3D11Texture2D* back_buffer = nullptr;
    if (FAILED(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer)))) {
        return;
    }

    device_->CreateRenderTargetView(back_buffer, nullptr, &render_target_);
    back_buffer->Release();
}

bool Renderer::InitializeImGui(IDXGISwapChain* swap_chain) {
    if (imgui_ready_.load()) {
        return true;
    }

    if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<void**>(&device_)))) {
        return false;
    }

    device_->GetImmediateContext(&context_);

    DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
    swap_chain->GetDesc(&swap_chain_desc);
    window_ = swap_chain_desc.OutputWindow;

    CreateRenderTarget(swap_chain);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplWin32_Init(window_);
    ImGui_ImplDX11_Init(device_, context_);
    Menu::Setup();

    original_wnd_proc_ = reinterpret_cast<WndProcFn>(
        SetWindowLongPtrA(window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WndProcHook))
    );

    imgui_ready_.store(true);
    Menu::AddLog("ImGui initialized");

    return true;
}

void Renderer::RenderFrame(IDXGISwapChain* swap_chain) {
    if (shutting_down_.load()) {
        return;
    }

    if (!imgui_ready_.load()) {
        if (!InitializeImGui(swap_chain)) {
            return;
        }
    }

    if (render_target_ == nullptr) {
        CreateRenderTarget(swap_chain);
    }

    ID3D11RenderTargetView* previous_rtv = nullptr;
    ID3D11DepthStencilView* previous_dsv = nullptr;
    context_->OMGetRenderTargets(1, &previous_rtv, &previous_dsv);

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
        Menu::Toggle();
    }

    ImGui::GetIO().MouseDrawCursor = Menu::IsVisible();
    Menu::Render();
    Aim::RenderOverlay();

    if (!Menu::IsVisible()) {
        ImGui::SetNextWindowBgAlpha(0.4f);
        ImGui::Begin("##watermark", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_NoFocusOnAppearing);
        ImGui::Text("cs2-client | INSERT");
        ImGui::End();
    }

    ImGui::Render();
    context_->OMSetRenderTargets(1, &render_target_, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    context_->OMSetRenderTargets(1, &previous_rtv, previous_dsv);
    if (previous_rtv != nullptr) {
        previous_rtv->Release();
    }
    if (previous_dsv != nullptr) {
        previous_dsv->Release();
    }
}

HRESULT __stdcall Renderer::PresentHook(
    IDXGISwapChain* swap_chain,
    UINT sync_interval,
    UINT flags
) {
    if (!shutting_down_.load()) {
        RenderFrame(swap_chain);
    }

    return original_present_(swap_chain, sync_interval, flags);
}

HRESULT __stdcall Renderer::ResizeBuffersHook(
    IDXGISwapChain* swap_chain,
    UINT buffer_count,
    UINT width,
    UINT height,
    DXGI_FORMAT new_format,
    UINT swap_chain_flags
) {
    if (imgui_ready_.load()) {
        ReleaseRenderTarget();
        ImGui_ImplDX11_InvalidateDeviceObjects();
    }

    const HRESULT result = original_resize_buffers_(
        swap_chain,
        buffer_count,
        width,
        height,
        new_format,
        swap_chain_flags
    );

    if (SUCCEEDED(result) && imgui_ready_.load()) {
        CreateRenderTarget(swap_chain);
        ImGui_ImplDX11_CreateDeviceObjects();
    }

    return result;
}

LRESULT __stdcall Renderer::WndProcHook(
    HWND hwnd,
    UINT msg,
    WPARAM w_param,
    LPARAM l_param
) {
    if (Menu::IsVisible() && ImGui_ImplWin32_WndProcHandler(hwnd, msg, w_param, l_param)) {
        return TRUE;
    }

    return CallWindowProcA(original_wnd_proc_, hwnd, msg, w_param, l_param);
}

bool Renderer::Initialize() {
    if (MH_Initialize() != MH_OK) {
        return false;
    }

    void* present_address = nullptr;
    void* resize_address = nullptr;
    if (!GetSwapChainVtable(&present_address, &resize_address)) {
        MH_Uninitialize();
        return false;
    }

    present_target_ = present_address;
    resize_target_ = resize_address;

    if (MH_CreateHook(present_target_, &PresentHook, reinterpret_cast<void**>(&original_present_)) != MH_OK) {
        MH_Uninitialize();
        return false;
    }

    if (MH_CreateHook(resize_target_, &ResizeBuffersHook, reinterpret_cast<void**>(&original_resize_buffers_)) != MH_OK) {
        MH_Uninitialize();
        return false;
    }

    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        MH_Uninitialize();
        return false;
    }

    hook_ready_.store(true);
    return true;
}

void Renderer::Shutdown() {
    if (shutting_down_.exchange(true)) {
        return;
    }

    if (hook_ready_.load()) {
        MH_DisableHook(MH_ALL_HOOKS);
        MH_RemoveHook(present_target_);
        MH_RemoveHook(resize_target_);
        MH_Uninitialize();
        hook_ready_.store(false);
    }

    if (imgui_ready_.load()) {
        if (window_ != nullptr && original_wnd_proc_ != nullptr) {
            SetWindowLongPtrA(window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc_));
            original_wnd_proc_ = nullptr;
        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        ReleaseRenderTarget();

        if (context_ != nullptr) {
            context_->Release();
            context_ = nullptr;
        }

        if (device_ != nullptr) {
            device_->Release();
            device_ = nullptr;
        }

        Menu::Shutdown();
        imgui_ready_.store(false);
    }
}
