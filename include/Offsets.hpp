#pragma once

#include <cstdint>

namespace Offsets {
    inline std::uintptr_t entity_list = 0;
    inline std::uintptr_t local_player = 0;
    inline std::uintptr_t view_angles = 0;
    inline std::uintptr_t view_matrix = 0;
    inline std::uintptr_t glow_object_manager = 0;

    inline std::uintptr_t m_vec_origin = 0x88;
    inline std::uintptr_t m_vec_view_offset = 0x108;
    inline std::uintptr_t m_i_team_num = 0x3BF;
    inline std::uintptr_t m_i_health = 0x32C;
    inline std::uintptr_t m_iGlowIndex = 0;
}
