#pragma once

#include <d3d11.h>
#include <dxgi.h>

#include <atomic>

class Renderer {
public:
    static bool Initialize();
    static void Shutdown();

private:
    static bool GetSwapChainVtable(void** out_present, void** out_resize_buffers);
    static HRESULT __stdcall PresentHook(
        IDXGISwapChain* swap_chain,
        UINT sync_interval,
        UINT flags
    );
    static HRESULT __stdcall ResizeBuffersHook(
        IDXGISwapChain* swap_chain,
        UINT buffer_count,
        UINT width,
        UINT height,
        DXGI_FORMAT new_format,
        UINT swap_chain_flags
    );
    static LRESULT __stdcall WndProcHook(
        HWND hwnd,
        UINT msg,
        WPARAM w_param,
        LPARAM l_param
    );
    static bool InitializeImGui(IDXGISwapChain* swap_chain);
    static void RenderFrame(IDXGISwapChain* swap_chain);
    static void ReleaseRenderTarget();
    static void CreateRenderTarget(IDXGISwapChain* swap_chain);

    using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
    using ResizeBuffersFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    using WndProcFn = LRESULT(__stdcall*)(HWND, UINT, WPARAM, LPARAM);

    static PresentFn original_present_;
    static ResizeBuffersFn original_resize_buffers_;
    static WndProcFn original_wnd_proc_;

    static ID3D11Device* device_;
    static ID3D11DeviceContext* context_;
    static ID3D11RenderTargetView* render_target_;
    static HWND window_;
    static void* present_target_;
    static void* resize_target_;

    static std::atomic<bool> imgui_ready_;
    static std::atomic<bool> hook_ready_;
    static std::atomic<bool> shutting_down_;
};
