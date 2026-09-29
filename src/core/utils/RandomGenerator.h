#pragma once

class RandomGenerator {
public:
    static int getInt(int min, int max);
    static int getInteger(int min, int max) { return getInt(min, max); }
    static bool rollChance(int successPercentage);

    // Metodos legados para compatibilidade retroativa
    static int getInteiro(int min, int max) { return getInt(min, max); }
    static bool rolarChance(int porcentagemSucesso) { return rollChance(porcentagemSucesso); }
};
