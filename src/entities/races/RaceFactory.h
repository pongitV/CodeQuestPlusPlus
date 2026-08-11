#pragma once

#include <memory>
#include "RaceBase.h"

class FabricaRacas {
public:
    static std::unique_ptr<RaceBase> createRace(RaceType tipo);
    static std::vector<RaceType> obterRacasJogaveis();

    // English Alias
    static std::vector<RaceType> getPlayableRaces() { return obterRacasJogaveis(); }
};

using RaceFactory = FabricaRacas;
