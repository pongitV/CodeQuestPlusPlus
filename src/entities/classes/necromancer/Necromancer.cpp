#include "Necromancer.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../systems/combat/Combat.h"
#include "../../../core/state/Status.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../core/utils/InputControl.h"
#include "../../races/RaceBase.h"
#include "../../../core/utils/Color.h"

// --- INFORMAÇÕES DA CLASSE ---
std::string Necromancer::getClassName() const {
    return "Necromancer";
}

const std::vector<std::string>& Necromancer::getClassMenuAppearance() const {
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Necromancer::getClassAttributes() const {
    return {-20, 5, 5, 3, 10, 10, 20};
}

std::vector<std::unique_ptr<Item>> Necromancer::getClassEquipment() const {
    auto equipment = ItemFactory::criarKitPocoes();
    equipment.push_back(ItemFactory::criarItem(ItemID::CajadoOsso));
    equipment.push_back(ItemFactory::criarItem(ItemID::RoupasRitualista));
    return equipment;
}

// --- PASSIVA DA CLASSE ---
std::string Necromancer::getClassPassiveName() const {
    return "Toque Necrotico";
}

std::string Necromancer::getClassPassiveDescription() const {
    return "Ataques aplicam Necrose, causando 5% da Health Max. do alvo como damage por 3 turnos.\n"
           "Ao derrotar um enemy, coleta sua alma.";
}

void Necromancer::executeAttackWithClassPassive(Character* attacker, Character* defender, int baseDamage, int piercingDamage, std::vector<std::unique_ptr<Character>>& enemyList, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool applyPassive) {
    // Comportamento padrão: apenas ataca o alvo principal ou todos se a arma for de área
    ClassBase::executeAttackWithClassPassive(attacker, defender, baseDamage, piercingDamage, enemyList,
        [&](Character* atk, Character* def, int dmg, int perf) {
            // Callback para aplicar o dano e depois o efeito da passiva
            applyDamage(atk, def, dmg, perf);
            if (def->obterVida() > 0 && applyPassive) {
                int necrosisDamage = static_cast<int>(def->obterVidaMaxima() * 0.05);
                if (necrosisDamage < 1) necrosisDamage = 1;
                def->adicionarEfeito(std::make_unique<NecrosisEffect>(3, necrosisDamage));
                std::string msg = DialogFunctions::formatarMsgHabilidade("Necrose! " + def->getName() + " perdera " + std::to_string(necrosisDamage) + " de HP por 3 turnos.", Color::MAGENTA);
                this->notifyCombatMessage(msg, msg);
            }
        }, applyPassive);
}

// --- HABILIDADE DA CLASSE ---
std::string Necromancer::getClassAbilityCooldownDescription() const {
    return "Recarga: Nenhuma (consome 1 alma).";
}

std::string Necromancer::getClassAbilityName() const {
    return "Invocacao de Morto-Vivo";
}

std::string Necromancer::getClassAbilityDescription() const {
    return "Usa uma alma para invocar um clone com 80% dos attributes (Chefes 60%). Max: 3 lacaios.\nLacaios perdem 15% de sua Health Max a cada turno do jogador.";
}

void Necromancer::useClassAbility(Combat* combat, Character* userCharacter, std::vector<Character*>& /*enemyList*/) {
    bool hasMiniBoss = false;
    int minionCount = 0;
    for (const auto& ally : combat->obterAliadosVivosRaw()) {
        if (ally->isMinion()) {
            minionCount++;
            if (ally->isBoss()) {
                hasMiniBoss = true;
            }
        }
    }

    if (hasMiniBoss) {
        std::string msg = DialogFunctions::formatarMsgSistema("Seu Morto-Vivo Chefe exige todo o seu controle! Nao e possivel invocar mais lacaios.", Color::RED);
        userCharacter->definirHabilidadeCancelada(true);
        return;
    }

    if (minionCount >= 3) {
        std::string msg = DialogFunctions::formatarMsgSistema("Limite maximo de 3 lacaios atingido!", Color::RED);
        userCharacter->definirHabilidadeCancelada(true);
        return;
    }

    if (userCharacter->obterNumeroDeAlmas() == 0) {
        std::string msg = DialogFunctions::formatarMsgSistema("Voce nao possui almas para invocar!", Color::RED);
        userCharacter->definirHabilidadeCancelada(true);
        return;
    }

    int maxPossible = std::min(3 - minionCount, static_cast<int>(userCharacter->obterNumeroDeAlmas()));

    std::vector<std::string> qtyOptions;
    for (int i = 1; i <= maxPossible; ++i) {
        if (i == 1) qtyOptions.push_back("1 Morto-Vivo");
        else qtyOptions.push_back(std::to_string(i) + " Mortos-Vivos (Enemy atua imediatamente)");
    }
    qtyOptions.push_back("Cancelar");

    int chosenQty = InputControl::readMenuSelectionWithArrows(qtyOptions, false, TelaCombate::margemCombate());
    if (chosenQty == static_cast<int>(qtyOptions.size()) - 1 || chosenQty == -1) {
        userCharacter->definirHabilidadeCancelada(true);
        return;
    }

    int amountToSummon = chosenQty + 1;
    std::vector<Character*> newlySummonedMinions;

    for (int i = 0; i < amountToSummon; ++i) {
        std::vector<std::string> options;
        auto& souls = userCharacter->obterAlmas();

        struct SoulGroup {
            std::string name;
            RaceType type;
            bool isBoss;
            int amount;
            int firstIndex;
        };
        std::vector<SoulGroup> groups;

        for (size_t j = 0; j < souls.size(); ++j) {
            bool found = false;
            for (auto& g : groups) {
                if (g.name == souls[j]->getName()) {
                    g.amount++;
                    found = true;
                    break;
                }
            }
            if (!found) groups.push_back({souls[j]->getName(), souls[j]->obterRaceType(), souls[j]->isBoss(), 1, static_cast<int>(j)});
        }

        for (const auto& g : groups) {
            std::string prefix = "";
            std::string color = "";
            
            if (g.isBoss) {
                if (g.type == RaceType::Mahoraga) {
                    prefix = "[CHEFE] ";
                    color = "";
                } else {
                    prefix = "[MINI-CHEFE] ";
                    color = "";
                }
            }
            
            options.push_back(color + std::to_string(g.amount) + "x " + prefix + "Morto-Vivo de " + g.name + "");
        }
        options.push_back("Cancelar Restante");

        int choice = InputControl::readMenuSelectionWithArrows(options, false, TelaCombate::margemCombate());

        if (choice == static_cast<int>(options.size()) - 1 || choice == -1) {
            if (i == 0) {
                userCharacter->definirHabilidadeCancelada(true);
                return;
            }
            break; // Para as invocações mas mantém as que já foram feitas
        }

        int realIndexToRemove = groups[choice].firstIndex;
        auto minion = userCharacter->removerAlma(realIndexToRemove);
        std::string originalName = minion->getName();
        
        double scaleFactor = 0.8;
        if (minion->isBoss()) {
            scaleFactor = 0.6;
        }
        
        minion->escalarAtributos(scaleFactor);
        minion->setAsMinion(true);
        minion->alterarNome("Morto-Vivo (" + originalName + ")");

        std::string msg = DialogFunctions::formatarMsgHabilidade(userCharacter->getName() + " ergueu um Morto-Vivo de " + originalName + "!", Color::MAGENTA);
        notifyCombatMessage(msg, msg);

        Character* minionPtr = minion.get();
        bool wasBoss = minionPtr->isBoss();
        combat->adicionarAliadoEmCombate(std::move(minion));
        newlySummonedMinions.push_back(minionPtr);

        if (wasBoss) {
            if (i < amountToSummon - 1) {
                std::string msgBoss = DialogFunctions::formatarMsgSistema("A invocacao de um Chefe consumiu seu foco! Invocacoes adicionais canceladas.", Color::YELLOW);
                notifyCombatMessage(msgBoss, msgBoss);
            }
            break; // Interrompe o laço
        }
    }
    
    // Se invocou mais de um minion na mesma ação, eles saltam seu turno ("Stun") para que o inimigo atue de imediato!
    if (newlySummonedMinions.size() > 1) {
        std::string msg = DialogFunctions::formatarMsgSistema("A invocacao multipla exauriu seu controle! O turno enemy comecara imediatamente!", Color::LIGHT_RED);
        notifyCombatMessage(msg, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for (auto* m : newlySummonedMinions) {
            m->adicionarEfeito(std::make_unique<StunEffect>(1));
        }
    }
}
