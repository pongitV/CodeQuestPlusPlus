#pragma once

namespace Constants 
{
    // Evolucao e Experiencia
    constexpr int BASE_XP_TO_LEVEL_UP = 100;
    constexpr double XP_MULTIPLIER_PER_LEVEL = 1.5;
    constexpr double MAX_XP = 1000000000.0; // Evita integer overflow em niveis altos
    
    // Attributes de Nivel
    constexpr int HEALTH_GAIN_PER_LEVEL = 20;
    constexpr int STAT_GAIN_PER_LEVEL = 1;

    // Recompensas Padrao e Multiplicadores
    constexpr int INITIAL_GOLD_REWARD = 15;
    constexpr double BARD_HEAL_MULTIPLIER = 1.4;
}
