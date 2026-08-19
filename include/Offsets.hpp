#pragma once

#include <cstdint>

namespace Offsets {
    inline std::uintptr_t entity_list = 0x025D5B30;          // dwEntityList
    inline std::uintptr_t local_player = 0x023A1398;         // dwLocalPlayerPawn
    inline std::uintptr_t view_angles = 0x023F76A8;          // dwViewAngles
    inline std::uintptr_t view_matrix = 0x023CB6F0;          // dwViewMatrix
    inline std::uintptr_t glow_object_manager = 0x02397058;  // dwGlowManager

    inline std::uintptr_t m_vec_origin = 0x80;               // CGameSceneNode::m_vecOrigin (fallback origin offset)
    inline std::uintptr_t m_vec_view_offset = 0xE78;         // C_BaseModelEntity::m_vecViewOffset
    inline std::uintptr_t m_i_team_num = 0x3E7;              // C_BaseEntity::m_iTeamNum
    inline std::uintptr_t m_i_health = 0x34C;                // C_BaseEntity::m_iHealth
    inline std::uintptr_t m_Glow = 0xDE0;                    // C_BaseModelEntity::m_Glow
    inline std::uintptr_t m_bDormant = 0x103;                // CGameSceneNode::m_bDormant
    inline std::uintptr_t m_bGunGameImmunity = 0x3258;       // C_CSPlayerPawn::m_bGunGameImmunity
    inline std::uintptr_t m_bInBombZone = 0x5385;            // C_CSPlayerPawn::m_bInBombZone
    inline std::uintptr_t m_bInHostageRescueZone = 0x5384;   // C_CSPlayerPawn::m_bInHostageRescueZone
    inline std::uintptr_t m_pHostageServices = 0x5272;       // C_CSPlayerPawn::m_pHostageServices
    inline std::uintptr_t m_hCarriedHostage = 72;            // CCSPlayer_HostageServices::m_hCarriedHostage
    inline std::uintptr_t m_bIsScoped = 0x7288;              // C_CSPlayerPawn::m_bIsScoped
    inline std::uintptr_t m_pCameraServices = 0x1240;        // C_BasePlayerPawn::m_pCameraServices
    inline std::uintptr_t m_vec_cs_view_punch = 0x48;        // CPlayer_CameraServices::m_vecCsViewPunchAngle
    inline std::uintptr_t m_pWeaponServices = 0x1208;        // C_BasePlayerPawn::m_pWeaponServices
    inline std::uintptr_t m_hActiveWeapon = 0x60;            // CPlayer_WeaponServices::m_hActiveWeapon
    inline std::uintptr_t m_iRecoilIndex = 0x6140;           // C_CSWeaponBase::m_iRecoilIndex
    inline std::uintptr_t m_flRecoilIndex = 0x6144;          // C_CSWeaponBase::m_flRecoilIndex
    inline std::uintptr_t m_fAccuracyPenalty = 0x6128;       // C_CSWeaponBase::m_fAccuracyPenalty
    inline std::uintptr_t m_flTurningInaccuracy = 0x6124;     // C_CSWeaponBase::m_flTurningInaccuracy
    inline std::uintptr_t m_flTurningInaccuracyDelta = 0x6108; // C_CSWeaponBase::m_flTurningInaccuracyDelta
    inline std::uintptr_t m_bInReload = 0x6164;               // C_CSWeaponBase::m_bInReload
    inline std::uintptr_t m_fFlags = 0x3F4;                   // C_BaseEntity::m_fFlags
    inline std::uintptr_t planted_c4 = 0x02373888;           // dwPlantedC4
    inline std::uintptr_t m_flC4Blow = 4560;                 // C_PlantedC4::m_flC4Blow
    inline std::uintptr_t m_bBombTicking = 4512;             // C_PlantedC4::m_bBombTicking
    inline std::uintptr_t m_flTimerLength = 4568;            // C_PlantedC4::m_flTimerLength
    inline std::uintptr_t m_bBombDefused = 4596;             // C_PlantedC4::m_bBombDefused
}
