#include "Bard.h"

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/Constants.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../core/utils/InputControl.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../core/utils/Color.h"

// Informacoes da classe
std::string Bard::getClassName() const 
{
     return "Bard"; 
}

const std::vector<std::string>& Bard::getClassMenuAppearance() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Bard::getClassAttributes() const
{
    return { 0, 10, 10, 3, 10, 10, 10};
}

std::vector<std::unique_ptr<Item>> Bard::getClassEquipment() const 
{
    auto equipment = ItemFactory::criarKitPocoes();
    
    equipment.push_back(ItemFactory::criarItem(ItemID::ViolaoEncantado));
    equipment.push_back(ItemFactory::criarItem(ItemID::CapaMagica));
    equipment.push_back(ItemFactory::criarItem(ItemID::TrajeNobre));
    return equipment;
}

// Passiva da classe
std::string Bard::getClassPassiveName() const 
{ 
    return "Touch the sky"; 
}

std::string Bard::getClassPassiveDescription() const 
{ 
    return "Curas e buffs recebidos sao 40% mais fortes."; 
}

int Bard::processBardPassiveHealing(int baseHeal) const 
{
    return static_cast<int>(baseHeal * Constants::BARD_HEAL_MULTIPLIER);
}

double Bard::processBardPassiveBuffMultiplier(double baseMultiplier) const 
{
    if (baseMultiplier > 1.0) return 1.0 + (baseMultiplier - 1.0) * 1.4;
    return baseMultiplier;
}

// Habilidade da classe
std::string Bard::getClassAbilityCooldownDescription() const 
{ 
    return "Recarga: 3 turnos (Individuais)."; 
}

std::string Bard::getClassAbilityName() const 
{ 
    return "Sinfonia do Bard"; 
}

std::string Bard::getClassAbilityDescription() const 
{ 
    return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; 
}

void Bard::useClassAbility(Combat* /*combate*/, Character* userCharacter, std::vector<Character*>& /*listaInimigos*/)
{
    struct SubAbility {
        AbilityID id;
        std::string name;
        std::string description;
        std::function<void(Character*)> action;
    };

    const std::array<SubAbility, 3> abilities = {{
        { AbilityID::FlashingLights, "Flashing lights", "Cura e pula o turno", [this](Character* character) {
            character->definirPularTurnoInimigo(true);
            int heal = static_cast<int>((character->getWisdom() * 2) + (character->obterVidaMaxima() * 0.15));
            character->modificarVida(heal);
            character->definirCooldown(AbilityID::FlashingLights, 3);
            std::string msg = DialogFunctions::formatarMsgHabilidade("!Flashing lights! Voce recuperou " + std::to_string(heal) + " HP e encantou os enemies!", Color::GREEN);
            this->notifyCombatMessage(msg, msg);
        }},
        { AbilityID::OnSight, "On sight", "1.5x Damage no proximo ataque", [this](Character* character) {
            character->definirMultiplicador(1.5);
            character->definirCooldown(AbilityID::OnSight, 3);
            std::string msg = DialogFunctions::formatarMsgHabilidade(character->getName() + " tocou 'On sight'! Proximo ataque com 1.5x damage!");
            this->notifyCombatMessage(msg, msg);
        }},
        { AbilityID::ThroughTheWire, "Through the wire", "Metade do damage recebido", [this](Character* character) {
            character->adicionarEfeito(std::make_unique<HalfDamageEffect>(1));
            character->definirCooldown(AbilityID::ThroughTheWire, 3);
            std::string msg = DialogFunctions::formatarMsgHabilidade("!Through the wire! Voce esta protegido contra metade do damage recebido!", Color::CYAN);
            this->notifyCombatMessage(msg, msg);
        }}
    }};

    std::vector<std::string> abilityOptions;
    for (size_t i = 0; i < abilities.size(); ++i) {
        int cd = userCharacter->obterRecargaHabilidade(abilities[i].id);
        abilityOptions.push_back(abilities[i].name + " (" + abilities[i].description + " | Recarga: " + std::to_string(cd) + ")");
    }
    abilityOptions.push_back("CANCELAR");

    int choice = InputControl::readMenuSelectionWithArrows(abilityOptions, false, TelaCombate::margemCombate());

    if (choice == static_cast<int>(abilities.size())) {
        userCharacter->definirHabilidadeCancelada(true);
        return;
    }
    
    const auto& hab = abilities[choice];
    int cd = userCharacter->obterRecargaHabilidade(hab.id);
    if (checkAndReportCooldown(userCharacter, cd, hab.name)) return;

    hab.action(userCharacter);
}
