#include "NPCGenericKnight.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include "../../classes/warrior/Warrior.h"
#include "../../races/human/Human.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../systems/inventory/items/MaterialItem.h"
#include "../../../systems/combat/Combat.h"
#include "../../../core/state/EnemyCreator.h"
#include "../../../ui/screens/menu/ScreenMenu.h"
#include "../../../core/utils/RendererProvider.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../systems/progress/Diary.h"
#include "../../../systems/progress/Progression.h"
#include "../../../systems/progress/ProgressionFlags.h"
#include "../../../core/utils/InputControl.h"
#include "../../../maps/control/MapController.h"
#include "NPCGenericKnightLayout.h"
#include "../../../core/utils/Color.h"

namespace {
    // Classes e funcoes auxiliares
    Item* findByName(Inventory* inv, const std::string& name) {
        for (auto* item : inv->getAllItems()) {
            if (item->getItemName() == name) return item;
        }
        return nullptr;
    }

    class KnightClass : public Warrior {
    public:
        std::string getClassName() const override { return "Cavaleiro Real"; }
        
        std::vector<std::unique_ptr<Item>> getClassEquipment() const override {
            std::vector<std::unique_ptr<Item>> equipmentList;
            Warrior base;
            for (auto& item : base.getClassEquipment()) {
                if (item->getType() == EquipmentType::Shield || item->getType() == EquipmentType::Consumable) {
                    equipmentList.push_back(std::move(item));
                }
            }
            equipmentList.push_back(ItemFactory::createItem(ItemID::EspadaCavaleiro));
            return equipmentList;
        }
    };

    class KnightRace : public Human {
    public:
        const std::vector<std::string>& getRaceAppearance() const override {
            return NPCGenericKnightLayouts::knightArt;
        }
    };

    bool findNearbyTroll(const std::vector<std::string>& map, int startX, int startY, int& outX, int& outY) {
        for (int dy = -2; dy <= 2; ++dy) {
            for (int dx = -5; dx <= 5; ++dx) {
                int y = startY + dy;
                int x = startX + dx;
                if (y >= 0 && y < static_cast<int>(map.size()) && x >= 0 && x < static_cast<int>(map[y].size())) {
                    if (map[y][x] == 'T') {
                        outX = x;
                        outY = y;
                        return true;
                    }
                }
            }
        }
        return false;
    }
}

// Criacao do NPC
std::unique_ptr<Character> NPCGenericKnight::createKnight(const std::string& name) {
    auto knight = std::make_unique<Character>(name, std::make_unique<KnightRace>(), std::make_unique<KnightClass>());
    std::string armorName = ItemFactory::getNameFromID(ItemID::ArmaduraCavaleiro);
    std::string swordName = ItemFactory::getNameFromID(ItemID::EspadaCavaleiro);
    knight->getInventory()->addItem(ItemFactory::createItem(ItemID::ArmaduraCavaleiro));
    knight->equipItem(findByName(knight->getInventory(), armorName));
    knight->equipItem(findByName(knight->getInventory(), swordName));
    knight->calculateAttributes();
    knight->modifyHealth(knight->getMaxHealth());
    return knight;
}

// Interacao
void NPCGenericKnight::interact(Character* currentPlayer, bool& trollDefeated, bool& invitationReceived, int /*larguraTerminal*/, std::vector<std::string>& currentMapMatrix, bool explorationActive, const std::function<void()>& restoreScreen, char targetCell, int nextX, int nextY) {
    Diary::instance().registerNPC("Cavaleiro Real");
    if (!trollDefeated && (targetCell == 'T' || targetCell == 'C')) {
        int trollPosX = -1, trollPosY = -1;
        
        if (targetCell == 'T') {
            trollPosX = nextX;
            trollPosY = nextY;
        } else if (targetCell == 'C') {
            findNearbyTroll(currentMapMatrix, nextX, nextY, trollPosX, trollPosY);
        }

        if (trollPosX == -1) {
            if (RendererProvider::get()) RendererProvider::get()->iniciarInteracaoPopup();
            std::vector<std::string> lines = {
                "Ainda temos invasores no reino!",
                "voce precisa de permissao se nao quiser ser",
                "tratado como invasor tambem...",
                "Nos ajude a derrotar todos e podemos",
                "garantir sua entrada no reino!"
            };
            if (RendererProvider::get()) RendererProvider::get()->displayPopup("CAVALEIROS REAIS", lines, Color::GRAY, NPCGenericKnightLayouts::knightArt);
            return;
        }

        if (RendererProvider::get()) RendererProvider::get()->iniciarInteracaoPopup();
        std::vector<std::string> lines = {
            "Viajante! Este Troll bloqueia a passagem.",
            "Nossas strengths estao se esgotando!",
            "Nos ajude a derrota-lo e o recompensaremos!"
        };
        
        int choice = 1; // Default
        InputControl::executarLoopMenuPopup(
            [&]() { return std::vector<std::string>{""}; },
            [&]() { return std::vector<std::string>{"Ajudar os Cavaleiros", "Recuar"}; },
            [&](const std::string& op) {
                if (op == "Ajudar os Cavaleiros") choice = 0;
                else choice = 1;
                return false; // Exit loop after choice
            },
            "PEDIDO DE AJUDA", Color::GRAY, NPCGenericKnightLayouts::knightArt
        );

        if (choice == 0) {
            Diary::instance().registerQuestAccepted("cavaleiro_trolls");
            std::vector<std::unique_ptr<Character>> allies;
            allies.push_back(createKnight("Cavaleiro Real 1"));
            allies.push_back(createKnight("Cavaleiro Real 2"));
            
            std::vector<std::unique_ptr<Character>> enemies;
            auto trolls = EnemyCreator::createTrollEnemy(1);
            if (!trolls.empty()) enemies.push_back(std::move(trolls[0])); 
            
            Combat combat(currentPlayer, std::move(enemies));
            combat.addCombatAllies(std::move(allies));
            if (MapControllera::isExploracao3DAtiva()) {
                combat.setContexto3D(
                    true, 
                    currentMapMatrix, 
                    MapControllera::obterPosCamera3DX(), 
                    MapControllera::obterPosCamera3DY(), 
                    MapControllera::obterAnguloCamera3D(), 
                    MapControllera::obterTituloMapaAtual()
                );
            }
            combat.startCombat();
            
            if (currentPlayer->getHealth() > 0) {
                currentMapMatrix[trollPosY][trollPosX] = '.';
                
                int remainingTrolls = 0;
                for (const auto& row : currentMapMatrix) {
                    remainingTrolls += std::count(row.begin(), row.end(), 'T');
                }
                if (remainingTrolls == 0) {
                    trollDefeated = true;
                    Progression::instance().setFlag(Flags::PonteReino_TrollDerrotado, true);
                }
            }
        }
        if (explorationActive && !MapControllera::isExploracao3DAtiva()) restoreScreen();
    } else if (targetCell == 'C') {
        InputControl::executarLoopMenuPopup(
            [&]() {
                if (!invitationReceived) {
                    return std::vector<std::string>{""};
                } else {
                    return std::vector<std::string>{""};
                }
            },
            [&]() { return std::vector<std::string>{"Conversar", "Missoes do Cavaleiro", "VOLTAR"}; },
            [&](const std::string& op) {
                if (op == "Conversar") {
                    if (!invitationReceived) {
                    } else {
                    }
                    return true;
                } else if (op == "Missoes do Cavaleiro") {
                    if (!invitationReceived) {
                        int questChoice = -1;
                        InputControl::executarLoopMenuPopup(
                            [&]() { return std::vector<std::string>{""}; },
                            [&]() { return std::vector<std::string>{"[M] Reportar Trolls derrotados", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "[M] Reportar Trolls derrotados") { questChoice = 0; return false; }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Color::GRAY, NPCGenericKnightLayouts::knightArt
                        );

                        if (questChoice == 0) {
                            std::vector<std::string> rewardLines = {
                                "Voce lutou bravamente e limpou o reino dos Trolls!",
                                "Como prometido, aqui esta a sua recompensa.",
                                "",
                                "Voce recebeu o [Convite Real]!"
                            };
                            currentPlayer->getInventory()->addItem(ItemFactory::createItem(ItemID::ConviteReal));
                            Diary::instance().registerItem("Convite Real");
                            Diary::instance().registerQuestCompleted("cavaleiro_trolls");
                            Progression::instance().setFlag(Flags::Vila_ConviteReal, true);
                            invitationReceived = true;
                        }
                    } else {
                        InputControl::executarLoopMenuPopup(
                            [&]() { return std::vector<std::string>{""}; },
                            [&]() { return std::vector<std::string>{"(Nenhuma missao disponivel)", "VOLTAR"}; },
                            [&](const std::string& subOp) {
                                if (subOp == "(Nenhuma missao disponivel)") {
                                }
                                return false;
                            },
                            "MISSOES - CAVALEIRO", Color::GRAY, NPCGenericKnightLayouts::knightArt
                        );
                    }
                    return true;
                } else if (op == "VOLTAR" || op == "Sair") {
                    return false;
                }
                return true;
            },
            "CAVALEIRO REAL", Color::GRAY, NPCGenericKnightLayouts::knightArt
        );
        if (explorationActive && !MapControllera::isExploracao3DAtiva()) restoreScreen();
    }
}
