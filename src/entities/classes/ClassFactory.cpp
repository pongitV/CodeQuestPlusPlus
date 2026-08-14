#include "ClassFactory.h"
#include "archer/Archer.h"
#include "bard/Bard.h"
#include "warrior/Warrior.h"
#include "mage/Mage.h"
#include "necromancer/Necromancer.h"

std::unique_ptr<ClassBase> ClassFactory::createClass(ClassType type) {
    switch (type) {
        case ClassType::Archer: return std::make_unique<Archer>();
        case ClassType::Bard: return std::make_unique<Bard>();
        case ClassType::Warrior: return std::make_unique<Warrior>();
        case ClassType::Mage: return std::make_unique<Mage>();
        case ClassType::Necromancer: return std::make_unique<Necromancer>();
        default: return nullptr;
    }
}

std::vector<ClassType> ClassFactory::getPlayableClasses() {
    return {ClassType::Archer, ClassType::Bard, ClassType::Warrior, ClassType::Mage, ClassType::Necromancer};
}
