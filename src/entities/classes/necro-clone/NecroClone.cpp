#include "NecroClone.h"

RaceClone::RaceClone(const std::string& name, const std::vector<std::string>& appearance) : originalName(name), originalAppearance(appearance) {}
std::string RaceClone::getRaceName() const { return originalName; }
RaceType RaceClone::getRaceType() const { return RaceType::None; }
const std::vector<std::string>& RaceClone::getRaceAppearance() const { return originalAppearance; }
Attributes RaceClone::getRaceAttributes() const { return {0,0,0,0,0,0,0}; }
std::string RaceClone::getRaceAbilityName() const { return "Ataque Basico"; }
std::string RaceClone::getRaceAbilityDescription() const { return "Ataca o enemy."; }

std::string PlayerClassClone::getClassName() const { return "Morto-Vivo"; }
ClassType PlayerClassClone::getClassType() const { return ClassType::None; }
const std::vector<std::string>& PlayerClassClone::getClassMenuAppearance() const { static std::vector<std::string> empty; return empty; }
Attributes PlayerClassClone::getClassAttributes() const { return {0,0,0,0,0,0,0}; }
std::vector<std::unique_ptr<Item>> PlayerClassClone::getClassEquipment() const { return {}; }
std::string PlayerClassClone::getClassPassiveName() const { return "Decomposicao"; }
std::string PlayerClassClone::getClassPassiveDescription() const { return "O corpo reanimado se decompoe continuamente, perdendo 15% da Health Maxima a cada turno do invocador."; }
std::string PlayerClassClone::getClassAbilityCooldownDescription() const { return ""; }
std::string PlayerClassClone::getClassAbilityName() const { return ""; }
std::string PlayerClassClone::getClassAbilityDescription() const { return ""; }
void PlayerClassClone::useClassAbility(Combat*, Character*, std::vector<Character*>&) {}
