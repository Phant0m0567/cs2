#include <Windows.h>
#include <TlHelp32.h>
#include <cstdio>
#include <string>

static DWORD FindProcessId(const char* process_name) {
    const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32A entry{};
    entry.dwSize = sizeof(entry);
    if (!Process32FirstA(snapshot, &entry)) {
        CloseHandle(snapshot);
        return 0;
    }

    do {
        if (_stricmp(entry.szExeFile, process_name) == 0) {
            CloseHandle(snapshot);
            return entry.th32ProcessID;
        }
    } while (Process32NextA(snapshot, &entry));

    CloseHandle(snapshot);
    return 0;
}

int main(int argc, char* argv[]) {
    const char* process_name = "cs2.exe";
    const char* dll_name = "cs2-client.dll";

    if (argc > 1 && argv[1][0] != '\0') {
        process_name = argv[1];
    }
    if (argc > 2 && argv[2][0] != '\0') {
        dll_name = argv[2];
    }

    char full_dll_path[MAX_PATH] = {0};
    if (GetFullPathNameA(dll_name, sizeof(full_dll_path), full_dll_path, nullptr) == 0) {
        std::printf("Failed to resolve DLL path '%s'\n", dll_name);
        return 1;
    }

    const DWORD pid = FindProcessId(process_name);
    if (pid == 0) {
        std::printf("Process '%s' not found. Start CS2 first.\n", process_name);
        return 1;
    }

    const HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);
    if (process == nullptr) {
        std::printf("Failed to open process %u. Error %u\n", pid, GetLastError());
        return 1;
    }

    const SIZE_T path_length = std::strlen(full_dll_path) + 1;
    LPVOID remote_mem = VirtualAllocEx(process, nullptr, path_length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remote_mem == nullptr) {
        std::printf("Failed to allocate remote memory. Error %u\n", GetLastError());
        CloseHandle(process);
        return 1;
    }

    if (!WriteProcessMemory(process, remote_mem, full_dll_path, path_length, nullptr)) {
        std::printf("Failed to write DLL path to target process. Error %u\n", GetLastError());
        VirtualFreeEx(process, remote_mem, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    const HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    const auto load_library = reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel32, "LoadLibraryA"));
    if (load_library == nullptr) {
        std::printf("Failed to resolve LoadLibraryA. Error %u\n", GetLastError());
        VirtualFreeEx(process, remote_mem, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    const HANDLE thread = CreateRemoteThread(process, nullptr, 0, load_library, remote_mem, 0, nullptr);
    if (thread == nullptr) {
        std::printf("Failed to create remote thread. Error %u\n", GetLastError());
        VirtualFreeEx(process, remote_mem, 0, MEM_RELEASE);
        CloseHandle(process);
        return 1;
    }

    const DWORD wait_result = WaitForSingleObject(thread, 10000);
    if (wait_result != WAIT_OBJECT_0) {
        std::printf("Remote thread failed or timed out. Result %u\n", wait_result);
    } else {
        std::printf("Injected '%s' into '%s' (PID %u)\n", full_dll_path, process_name, pid);
    }

    VirtualFreeEx(process, remote_mem, 0, MEM_RELEASE);
    CloseHandle(thread);
    CloseHandle(process);
    return 0;
}
