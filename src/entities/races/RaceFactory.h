#pragma once

#include <memory>
#include <vector>
#include "RaceBase.h"

class RaceFactory {
public:
    static std::unique_ptr<RaceBase> createRace(RaceType type);
    static std::vector<RaceType> getPlayableRaces();

    // Compatibilidade legada
    static std::vector<RaceType> obterRacasJogaveis() { return getPlayableRaces(); }
};

using FabricaRacas = RaceFactory;
