#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include <windows.h>

class Memory {
public:
    static std::uintptr_t GetModuleBase(std::string_view module_name);
    static std::size_t GetModuleSize(std::string_view module_name);

    static std::optional<std::uintptr_t> FindPattern(
        std::uintptr_t start,
        std::size_t size,
        std::string_view pattern
    );

    static std::optional<std::uintptr_t> FindPatternInModule(
        std::string_view module_name,
        std::string_view pattern
    );

    template<typename T>
    static T* GetExport(HMODULE module, const char* export_name) {
        return reinterpret_cast<T*>(GetProcAddress(module, export_name));
    }

    template<typename T>
    static T* GetExport(std::string_view module_name, const char* export_name) {
        const HMODULE module = GetModuleHandleA(module_name.data());
        if (module == nullptr) {
            return nullptr;
        }
        return GetExport<T>(module, export_name);
    }

    template<typename T>
    static T Read(std::uintptr_t address) {
        return *reinterpret_cast<T*>(address);
    }

    template<typename T>
    static void Write(std::uintptr_t address, const T& value) {
        *reinterpret_cast<T*>(address) = value;
    }

    static bool IsReadable(std::uintptr_t address, std::size_t size);

private:
    struct PatternByte {
        std::uint8_t value = 0;
        bool wildcard = false;
    };

    static std::vector<PatternByte> ParsePattern(std::string_view pattern);
};
