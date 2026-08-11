#pragma once

namespace Flags {
    // Village Flags
    inline constexpr const char* Village_NPCs = "Village_NPCs";
    inline constexpr const char* Village_Enemies = "Village_Enemies";
    inline constexpr const char* Village_RoyalInvitation = "Village_RoyalInvitation";
    inline constexpr const char* Village_BjornRescued = "Village_BjornRescued";

    // Forest Flags
    inline constexpr const char* Forest_NPCs = "Forest_NPCs";
    inline constexpr const char* Forest_MorganaQuest = "Forest_MorganaQuest";
    inline constexpr const char* Forest_MahoragaDefeated = "Forest_MahoragaDefeated";
 
    // Kingdom Bridge Flags
    inline constexpr const char* KingdomBridge_TrollDefeated = "KingdomBridge_TrollDefeated";
    inline constexpr const char* KingdomBridge_NPCs = "KingdomBridge_NPCs";
    inline constexpr const char* KingdomBridge_Enemies = "KingdomBridge_Enemies";

    // Fast Travel Visit Flags
    inline constexpr const char* Visited_Forest = "Visited_Forest";
    inline constexpr const char* Visited_KingdomBridge = "Visited_KingdomBridge";
    inline constexpr const char* Visited_Kingdom = "Visited_Kingdom";
    inline constexpr const char* Discovered_Maps = "Discovered_Maps";

    // Compatibility Aliases
    inline constexpr const char* Vila_NPCs = Village_NPCs;
    inline constexpr const char* Vila_Inimigos = Village_Enemies;
    inline constexpr const char* Vila_ConviteReal = Village_RoyalInvitation;
    inline constexpr const char* Vila_BjornResgatado = Village_BjornRescued;
    inline constexpr const char* Floresta_NPCs = Forest_NPCs;
    inline constexpr const char* Floresta_MissaoMorgana = Forest_MorganaQuest;
    inline constexpr const char* Floresta_MahoragaDerrotado = Forest_MahoragaDefeated;
    inline constexpr const char* PonteReino_TrollDerrotado = KingdomBridge_TrollDefeated;
    inline constexpr const char* PonteReino_NPCs = KingdomBridge_NPCs;
    inline constexpr const char* PonteReino_Inimigos = KingdomBridge_Enemies;
    inline constexpr const char* Visitou_Floresta = Visited_Forest;
    inline constexpr const char* Visitou_PonteReino = Visited_KingdomBridge;
    inline constexpr const char* Visitou_Reino = Visited_Kingdom;
    inline constexpr const char* Mapas_Descobertos = Discovered_Maps;
}
