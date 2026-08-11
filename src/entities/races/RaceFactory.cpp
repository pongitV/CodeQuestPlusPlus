#include "RaceFactory.h"
#include "dwarf/Dwarf.h"
#include "elf/Elf.h"
#include "human/Human.h"
#include "orc/Orc.h"
// Adicione includes de enemies se necessário no futuro

std::unique_ptr<RaceBase> FabricaRacas::createRace(RaceType tipo) {
    switch (tipo) {
        case RaceType::Dwarf: return std::make_unique<Dwarf>();
        case RaceType::Elf: return std::make_unique<Elf>();
        case RaceType::Human: return std::make_unique<Human>();
        case RaceType::Ork: return std::make_unique<Ork>();
        default: return nullptr;
    }
}

std::vector<RaceType> FabricaRacas::obterRacasJogaveis() {
    return {RaceType::Dwarf, RaceType::Elf, RaceType::Human, RaceType::Ork};
}
