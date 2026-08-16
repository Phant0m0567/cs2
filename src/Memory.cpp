#include "Memory.hpp"

#include <cstring>
#include <cstdlib>
#include <psapi.h>

namespace {
HMODULE GetModuleHandleFromView(std::string_view module_name) {
    if (module_name.empty()) {
        return GetModuleHandleA(nullptr);
    }

    char name_buffer[260]{};
    if (module_name.size() >= sizeof(name_buffer)) {
        return nullptr;
    }

    std::memcpy(name_buffer, module_name.data(), module_name.size());
    return GetModuleHandleA(name_buffer);
}
}

std::uintptr_t Memory::GetModuleBase(std::string_view module_name) {
    return reinterpret_cast<std::uintptr_t>(GetModuleHandleFromView(module_name));
}

std::size_t Memory::GetModuleSize(std::string_view module_name) {
    const HMODULE module = GetModuleHandleFromView(module_name);
    if (module == nullptr) {
        return 0;
    }

    MODULEINFO module_info{};
    if (!GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(module_info))) {
        return 0;
    }

    return module_info.SizeOfImage;
}

std::vector<Memory::PatternByte> Memory::ParsePattern(std::string_view pattern) {
    std::vector<PatternByte> parsed;
    parsed.reserve(32);

    std::size_t index = 0;
    while (index < pattern.size()) {
        if (pattern[index] == ' ') {
            ++index;
            continue;
        }

        if (pattern[index] == '?') {
            parsed.push_back({0, true});
            if (index + 1 < pattern.size() && pattern[index + 1] == '?') {
                ++index;
            }
            index += 1;
            continue;
        }

        const char hex_pair[] = {
            pattern[index],
            (index + 1 < pattern.size()) ? pattern[index + 1] : '0',
            '\0'
        };

        parsed.push_back({static_cast<std::uint8_t>(std::strtoul(hex_pair, nullptr, 16)), false});
        index += 2;
    }

    return parsed;
}

std::optional<std::uintptr_t> Memory::FindPattern(
    std::uintptr_t start,
    std::size_t size,
    std::string_view pattern
) {
    const auto parsed = ParsePattern(pattern);
    if (parsed.empty() || size < parsed.size()) {
        return std::nullopt;
    }

    const auto* region = reinterpret_cast<const std::uint8_t*>(start);
    const std::size_t scan_limit = size - parsed.size();

    for (std::size_t offset = 0; offset <= scan_limit; ++offset) {
        bool matched = true;

        for (std::size_t byte_index = 0; byte_index < parsed.size(); ++byte_index) {
            const auto& pattern_byte = parsed[byte_index];
            if (!pattern_byte.wildcard && region[offset + byte_index] != pattern_byte.value) {
                matched = false;
                break;
            }
        }

        if (matched) {
            return start + offset;
        }
    }

    return std::nullopt;
}

std::optional<std::uintptr_t> Memory::FindPatternInModule(
    std::string_view module_name,
    std::string_view pattern
) {
    const std::uintptr_t base = GetModuleBase(module_name);
    const std::size_t size = GetModuleSize(module_name);

    if (base == 0 || size == 0) {
        return std::nullopt;
    }

    return FindPattern(base, size, pattern);
}

bool Memory::IsReadable(std::uintptr_t address, std::size_t size) {
    MEMORY_BASIC_INFORMATION memory_info{};
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &memory_info, sizeof(memory_info)) == 0) {
        return false;
    }

    if (memory_info.State != MEM_COMMIT) {
        return false;
    }

    if (memory_info.Protect == PAGE_NOACCESS || (memory_info.Protect & PAGE_GUARD) != 0) {
        return false;
    }

    const std::uintptr_t region_start = reinterpret_cast<std::uintptr_t>(memory_info.BaseAddress);
    const std::uintptr_t region_end = region_start + memory_info.RegionSize;

    return address >= region_start && (address + size) <= region_end;
}
