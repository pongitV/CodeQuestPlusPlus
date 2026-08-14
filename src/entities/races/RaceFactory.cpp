#include "RaceFactory.h"
#include "dwarf/Dwarf.h"
#include "elf/Elf.h"
#include "human/Human.h"
#include "orc/Orc.h"

std::unique_ptr<RaceBase> RaceFactory::createRace(RaceType type) {
    switch (type) {
        case RaceType::Dwarf: return std::make_unique<Dwarf>();
        case RaceType::Elf: return std::make_unique<Elf>();
        case RaceType::Human: return std::make_unique<Human>();
        case RaceType::Orc: return std::make_unique<Orc>();
        default: return nullptr;
    }
}

std::vector<RaceType> RaceFactory::getPlayableRaces() {
    return {RaceType::Dwarf, RaceType::Elf, RaceType::Human, RaceType::Orc};
}
