#pragma once

#include <cstdint>

namespace Offsets {
    inline std::uintptr_t entity_list = 0x2555050;          // dwEntityList
    inline std::uintptr_t local_player = 0x23AA118;         // dwLocalPlayerPawn
    inline std::uintptr_t view_angles = 0x23C01A8;          // dwViewAngles
    inline std::uintptr_t view_matrix = 0x23AF550;          // dwViewMatrix
    inline std::uintptr_t glow_object_manager = 0x23A6908;  // dwGlowManager
    inline std::uintptr_t planted_c4 = 0x2374898;           // dwPlantedC4

    inline std::uintptr_t m_vec_origin = 0x80;               // CGameSceneNode::m_vecOrigin (fallback origin offset)
    inline std::uintptr_t m_vec_view_offset = 0xE78;         // C_BaseModelEntity::m_vecViewOffset
    inline std::uintptr_t m_i_team_num = 0x3E7;              // C_BaseEntity::m_iTeamNum
    inline std::uintptr_t m_i_health = 0x34C;                // C_BaseEntity::m_iHealth
    inline std::uintptr_t m_unWeaponHash = 0x14FC;           // C_CSPlayerPawn::m_unWeaponHash
    inline std::uintptr_t m_i_armor_value = 0x1CA4;          // C_CSPlayerPawn::m_ArmorValue
    inline std::uintptr_t m_entity_spotted_state = 0x1C60;    // C_CSPlayerPawn::m_entitySpottedState
    inline std::uintptr_t m_Glow = 0xDE0;                    // C_BaseModelEntity::m_Glow
    inline std::uintptr_t m_bDormant = 0x103;                // CGameSceneNode::m_bDormant
    inline std::uintptr_t m_bGunGameImmunity = 0x3268;       // C_CSPlayerPawn::m_bGunGameImmunity
    inline std::uintptr_t m_bInBombZone = 0x1509;            // C_CSPlayerPawn::m_bInBombZone
    inline std::uintptr_t m_bInHostageRescueZone = 0x1508;   // C_CSPlayerPawn::m_bInHostageRescueZone
    inline std::uintptr_t m_pHostageServices = 0x1498;       // C_CSPlayerPawn::m_pHostageServices
    inline std::uintptr_t m_hCarriedHostage = 0x48;          // CCSPlayer_HostageServices::m_hCarriedHostage
    inline std::uintptr_t m_bIsScoped = 0x1C78;              // C_CSPlayerPawn::m_bIsScoped
    inline std::uintptr_t m_pCameraServices = 0x1240;        // C_BasePlayerPawn::m_pCameraServices
    inline std::uintptr_t m_vec_cs_view_punch = 0x48;        // CPlayer_CameraServices::m_vecCsViewPunchAngle
    inline std::uintptr_t m_pWeaponServices = 0x1208;        // C_BasePlayerPawn::m_pWeaponServices
    inline std::uintptr_t m_hActiveWeapon = 0x60;            // CPlayer_WeaponServices::m_hActiveWeapon
    inline std::uintptr_t m_iRecoilIndex = 0x17FC;           // C_CSWeaponBase::m_iRecoilIndex
    inline std::uintptr_t m_flRecoilIndex = 0x1800;          // C_CSWeaponBase::m_flRecoilIndex
    inline std::uintptr_t m_fAccuracyPenalty = 0x17F0;       // C_CSWeaponBase::m_fAccuracyPenalty
    inline std::uintptr_t m_flTurningInaccuracy = 0x17EC;     // C_CSWeaponBase::m_flTurningInaccuracy
    inline std::uintptr_t m_flTurningInaccuracyDelta = 0x17DC; // C_CSWeaponBase::m_flTurningInaccuracyDelta
    inline std::uintptr_t m_bInReload = 0x1814;               // C_CSWeaponBase::m_bInReload
    inline std::uintptr_t m_iClip1 = 0x1700;                 // C_BasePlayerWeapon::m_iClip1
    inline std::uintptr_t m_pReserveAmmo = 0x1708;           // C_BasePlayerWeapon::m_pReserveAmmo
    inline std::uintptr_t m_fFlags = 0x3F4;                   // C_BaseEntity::m_fFlags
    inline std::uintptr_t m_flC4Blow = 0x11D0;               // C_PlantedC4::m_flC4Blow
    inline std::uintptr_t m_bBombTicking = 0x11A0;           // C_PlantedC4::m_bBombTicking
    inline std::uintptr_t m_flTimerLength = 0x11D8;          // C_PlantedC4::m_flTimerLength
    inline std::uintptr_t m_bBombDefused = 0x11F4;           // C_PlantedC4::m_bBombDefused
}
