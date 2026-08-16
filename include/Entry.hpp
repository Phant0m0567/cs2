#pragma once

#include <windows.h>

class Entry {
public:
    static bool OnAttach(HMODULE module);
    static void OnDetach(bool process_terminating);

private:
    static DWORD WINAPI ThreadProc(LPVOID param);

    static void Initialize();
    static void Unload();
    static void RunLoop();

    static HMODULE module_;
    static HANDLE thread_;
    static volatile bool should_run_;
};
