#include "NecroClone.h"

RaceClone::RaceClone(const std::string& n, const std::vector<std::string>& a) : nomeOriginal(n), aparenciaOriginal(a) {}
std::string RaceClone::getRaceName() const { return nomeOriginal; }
RaceType RaceClone::obterRaceType() const { return RaceType::Nenhum; }
const std::vector<std::string>& RaceClone::getRaceAppearance() const { return aparenciaOriginal; }
Attributes RaceClone::getRaceAttributes() const { return {0,0,0,0,0,0,0}; }
std::string RaceClone::getRaceAbilityName() const { return "Ataque Basico"; }
std::string RaceClone::getRaceAbilityDescription() const { return "Ataca o enemy."; }

std::string PlayerClassClone::getNameClasse() const { return "Morto-Vivo"; }
ClassType PlayerClassClone::obterClassType() const { return ClassType::Nenhum; }
const std::vector<std::string>& PlayerClassClone::obterAparenciaClasseMenu() const { static std::vector<std::string> empty; return empty; }
Attributes PlayerClassClone::obterAtributosClasse() const { return {0,0,0,0,0,0,0}; }
std::vector<std::unique_ptr<Item>> PlayerClassClone::obterEquipamentoClasse() const { return {}; }
std::string PlayerClassClone::getNamePassivaClasse() const { return "Decomposicao"; }
std::string PlayerClassClone::obterDescricaoPassivaClasse() const { return "O corpo reanimado se decompoe continuamente, perdendo 15% da Health Maxima a cada turno do invocador."; }
std::string PlayerClassClone::obterRecargaHabilidadeClasse() const { return ""; }
std::string PlayerClassClone::getNameHabilidadeClasse() const { return ""; }
std::string PlayerClassClone::getClassAbilityDescription() const { return ""; }
void PlayerClassClone::useClassAbility(Combat*, Character*, std::vector<Character*>&) {}
