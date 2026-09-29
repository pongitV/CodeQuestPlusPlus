#include "InventoryCombat.h"
#include "../../core/d2d-context/D2DContext.h"
#include "../../core/window/GameWindow.h"
#include "../../ui/UIManager.h"
#include "../../entities/character/Character.h"
#include "Item.h"
#include "./equipment/WeaponEquipment.h"
#include "./equipment/ShieldEquipment.h"
#include "./equipment/ArmorEquipment.h"
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/ScreenBase.h"
#include "../../ui/screens/inventory/ScreenInventory.h"
#include "../../ui/screens/inventory/ScreenInventoryLayout.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../core/utils/DialogFunctions.h"
#include "InventoryController.h"
#include "../../rendering/raycaster/engine-raycaster/RaycasterFrame.h"
#include "../../rendering/raycaster/screens/utils/MenuD2DUtils.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "../../core/utils/Color.h"

enum InventoryState { MAIN, ARSENAL, CONSUMABLES, STORAGE, QUEST };

static int readInventoryPopupSelection(const std::string& title, const std::string& message, const std::vector<std::string>& text, const std::vector<std::string>& options) {
    // Renderizacao Direct2D
    auto renderCb = [&](UIDynamicBox& box, int selA, float optStartX, float optStartY, float pixelScale) {
        float lw = UIRenderer2D::LOGICAL_WIDTH;
        
        float currentY = optStartY;
        if (!message.empty()) {
            std::wstring wMsg = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(message));
            box.AddText(wMsg, lw/2.0f, currentY, 18.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
            currentY += 40.0f;
        }
        
        for (const auto& t : text) {
            std::wstring wT = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(t));
            box.AddText(wT, lw/2.0f, currentY, 16.0f, D2D1::ColorF(0.9f, 0.9f, 0.9f), true);
            currentY += 25.0f;
        }
        
        currentY += 30.0f;
        for (size_t i = 0; i < options.size(); ++i) {
            std::wstring wOp = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(options[i]));
            MenuRaycasterUtils::adicionarOpcaoMenu(box, wOp, lw/2.0f, currentY, ((int)i == selA), D2D1::ColorF(0.7f, 0.7f, 0.7f), true);
            currentY += 30.0f;
        }
    };

    int res = MenuRaycasterUtils::renderizarMenuPadrao(
        MenuRaycasterUtils::utf8_to_wstring(title),
        D2D1::ColorF(1.0f, 1.0f, 0.0f),
        options,
        {}, {}, MenuRaycasterUtils::PosicaoArte::NENHUMA, 1.0f,
        renderCb,
        nullptr,
        ArtesInventario::logoInventario,
        {}
    );
    return (res == -1) ? options.size() - 1 : res;
}

static void displayInventoryPopupMessage(const std::string& title, const std::vector<std::string>& text) {
    readInventoryPopupSelection(title, "", text, {"[ VOLTAR ]"});
}

static void displayItemResult(const ItemUsageInfo& info, Item* item, bool* turnConsumed) {
    switch (info.result) {
        case ItemResult::Error_TurnAlreadyUsed:
            displayInventoryPopupMessage("SISTEMA", {"Voce ja usou um item neste turno!"});
            break;
        case ItemResult::Error_BrokenShield: {
            std::string msg = DialogFunctions::formatarMsgSistema("O escudo [" + info.itemName + "] esta quebrado e nao pode ser equipado!", Color::RED);
            displayInventoryPopupMessage("SISTEMA", {msg});
            break;
        }
        case ItemResult::Error_Requirements:
            displayInventoryPopupMessage("SISTEMA", {info.extraMessage});
            break;
        case ItemResult::Unequipped:
            displayInventoryPopupMessage("SISTEMA", {info.itemName + " desequipado(a)!"});
            if (turnConsumed) *turnConsumed = true;
            displayInventoryPopupMessage("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ItemResult::Equipped:
            displayInventoryPopupMessage("SISTEMA", {info.itemName + " equipado(a)!"});
            if (turnConsumed) *turnConsumed = true;
            displayInventoryPopupMessage("SISTEMA", {"Turno gasto alterando um equipamento..."});
            break;
        case ItemResult::Used_Turn:
            if (turnConsumed) *turnConsumed = true;
            break;
        case ItemResult::Used_NoTurn:
            break;
        case ItemResult::Error_CannotUse:
            displayInventoryPopupMessage("SISTEMA", {InventoryController::getErrorMessage(item, turnConsumed != nullptr)});
            break;
        default: break;
    }
}

static int readInventoryPopupInteger(const std::string& title, const std::string& message, int min, int max) {
    std::string currentInput = "";
    // Renderizacao Direct2D
    while (true) {
        auto renderCb = [&](UIDynamicBox& box, int selA, float lw, float lh) {
            std::wstring wTit = MenuRaycasterUtils::utf8_to_wstring(title);
            box.SetTitle(wTit, D2D1::ColorF(1.0f, 1.0f, 0.0f));
            
            std::wstring wMsg = MenuRaycasterUtils::utf8_to_wstring(MenuRaycasterUtils::stripAnsi(message));
            box.AddText(wMsg, lw/2.0f, 90.0f, 18.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f), true);
            
            std::wstring wInput = MenuRaycasterUtils::utf8_to_wstring(" >> " + currentInput + "_");
            box.AddText(wInput, lw/2.0f, 130.0f, 20.0f, D2D1::ColorF(0.2f, 1.0f, 0.2f), true);
            
            box.AddText(L"[ENTER para confirmar]", lw/2.0f, 170.0f, 14.0f, D2D1::ColorF(0.6f, 0.6f, 0.6f), true);
        };
        
        auto* d2d = D2DContext::renderer;
        if (d2d) {
            UIDynamicBox box;
            float lw = UIRenderer2D::LOGICAL_WIDTH;
            float lh = UIRenderer2D::LOGICAL_HEIGHT;
            renderCb(box, 0, lw, lh);
            
            if (D2DContext::window) D2DContext::window->processarMensagens();
            InputControl::atualizarTeclas();
            
            d2d->obterRenderTarget()->BeginDraw();
            RaycasterFrame::restaurarUltimoQuadro();
            box.Render(d2d, D2D1::ColorF(0.05f, 0.05f, 0.08f, 0.95f), 0.95f, 2.0f, D2D1::ColorF(1.0f, 0.8f, 0.0f), 20.0f, lw/2.0f, lh/2.0f);
            d2d->obterRenderTarget()->EndDraw();
            d2d->apresentarBackbuffer();
        }
        
        char key = InputControl::lerTecla();
        if (key >= '0' && key <= '9') {
            currentInput += key;
        } else if (key == 8 && !currentInput.empty()) { // backspace
            currentInput.pop_back();
        } else if (key == '\r' || key == '\n') {
            if (currentInput.empty()) return min;
            int val = std::stoi(currentInput);
            if (val < min) return min;
            if (val > max) return max;
            return val;
        }
    }
}

void InventoryCombat::manageInventory(Character* currentPlayer, bool* turnConsumed)
{
    if (currentPlayer == nullptr) return;
    
    InventoryState state = MAIN;
    int currentSelection = 0;
    int subSelection = 0;
    bool running = true;
    
    std::vector<Item*> indexToItemMap;

    bool fullRedrawInv = true;

    while (running) {
        bool is3D = GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
        
        std::vector<std::string> lines;
        std::string boxTitle = "";
        std::vector<std::string> interactive;
        std::vector<int> realIndices;
        
        if (state == MAIN) {
            boxTitle = " MENU DE BOLSOS ";
            std::string pocketStr = "BOLSO: " + std::to_string(currentPlayer->getInventory()->getGold()) + " Moedas de Ouro [$$]";
            
            std::vector<std::string> baseOptions;
            if (currentPlayer->getQuickConsumable()) {
                int count = currentPlayer->getInventory()->countItem(currentPlayer->getQuickConsumable()->getItemName());
                baseOptions.push_back(std::string("[+] ") + "Acesso Rapido: " + currentPlayer->getQuickConsumable()->getItemName() + " (" + std::to_string(count) + "x)");
            }
            baseOptions.push_back("Arsenal de Equipamentos");
            baseOptions.push_back("Itens Consumiveis");
            baseOptions.push_back("Estoque e Materiais");
            baseOptions.push_back("Itens de Missao");
            baseOptions.push_back("");
            baseOptions.push_back(pocketStr);
            baseOptions.push_back("");
            baseOptions.push_back("VOLTAR");
            
            for (size_t i = 0; i < baseOptions.size(); ++i) {
                if (baseOptions[i].empty() || baseOptions[i].find("BOLSO:") != std::string::npos || baseOptions[i].substr(0, 3) == "   ") {
                    lines.push_back("   " + baseOptions[i]);
                } else {
                    interactive.push_back(baseOptions[i]);
                    realIndices.push_back(i);
                    if (interactive.size() - 1 == currentSelection) {
                        lines.push_back(std::string(" > ") + baseOptions[i]);
                    } else {
                        lines.push_back("   " + baseOptions[i]);
                    }
                }
            }
        } else if (state == ARSENAL) {
            boxTitle = " ARSENAL DE EQUIPAMENTOS ";

            indexToItemMap.clear();

            Item* weaponEq = currentPlayer->getWeapon();
            Item* armorEq = currentPlayer->getArmor();
            Item* shieldEq = currentPlayer->getShield();

            auto allItems = currentPlayer->getInventory()->getAllItems();
            std::vector<Item*> weapons, armors, shields;
            for (auto* item : allItems) {
                if (item == weaponEq || item == armorEq || item == shieldEq) continue;
                EquipmentType type = item->getType();
                if (type == EquipmentType::Weapon) weapons.push_back(item);
                else if (type == EquipmentType::Armor) armors.push_back(item);
                else if (type == EquipmentType::Shield) shields.push_back(item);
            }

            std::string divColor = "";
            std::string resetColor = "";

            auto addItem = [&](const std::string& name, Item* item) {
                int idx = (int)interactive.size();
                interactive.push_back(name);
                realIndices.push_back((int)indexToItemMap.size());
                indexToItemMap.push_back(item);
                if (idx == subSelection)
                    lines.push_back(std::string(" > ") + name);
                else
                    lines.push_back("   " + name);
            };

            auto addGroup = [&](const std::string& label, std::vector<Item*>& group) {
                if (group.empty()) return;
                lines.push_back(" " + divColor + "--- " + label + " ---" + resetColor);
                for (auto* item : group) {
                    auto groupedItems = currentPlayer->getInventory()->countItem(item->getItemName());
                    std::string prefix = (groupedItems > 1) ? std::to_string(groupedItems) + "x " : "";
                    addItem(prefix + item->getItemName(), item);
                }
                lines.push_back("");
            };

            // Equipados (nao interativo)
            lines.push_back(" " + divColor + "--- Equipados ---" + resetColor);
            bool hasEq = false;
            auto addEq = [&](const std::string& label, Item* item) {
                if (!item) return;
                hasEq = true;
                std::string name = item->getItemName();
                // tambem adiciona aos interativos para permitir selecao
                int idx = (int)interactive.size();
                interactive.push_back("(E) " + name);
                realIndices.push_back((int)indexToItemMap.size());
                indexToItemMap.push_back(item);
                if (idx == subSelection)
                    lines.push_back(std::string(" > ") + std::string("[E] ") + label + ": " + name);
                else
                    lines.push_back("   " + std::string("[E] ") + label + ": " + name);
            };
            addEq("Weapon", weaponEq);
            addEq("Armor", armorEq);
            addEq("Shield", shieldEq);
            if (!hasEq) lines.push_back("   " + std::string("(Nada equipado)") + resetColor);
            lines.push_back("");

            addGroup("Armas", weapons);
            addGroup("Armaduras", armors);
            addGroup("Escudos", shields);

            interactive.push_back("VOLTAR");
            realIndices.push_back(-1);
            if ((int)interactive.size() - 1 == subSelection)
                lines.push_back(" > VOLTAR");
            else
                lines.push_back("   VOLTAR");

        } else {
            int category = 0;
            if (state == CONSUMABLES) { boxTitle = " ITENS CONSUMIVEIS "; category = 1; }
            else if (state == STORAGE) { boxTitle = " ESTOQUE E MATERIAIS "; category = 2; }
            else if (state == QUEST) { boxTitle = " ITENS DE MISSAO "; category = 3; }

            auto items = TelaInventario::obterListaCategoria(currentPlayer, category, false);
            indexToItemMap.clear();

            if (!items.empty()) {
                for (const auto& p : items) {
                    interactive.push_back(p.first);
                    realIndices.push_back((int)indexToItemMap.size());
                    indexToItemMap.push_back(p.second);
                }
            }
            interactive.push_back("VOLTAR");
            realIndices.push_back(-1);
        }
        
        int totalOptions = interactive.size();
        int* selRef = (state == MAIN) ? &currentSelection : &subSelection;
        if (*selRef >= totalOptions && totalOptions > 0) *selRef = totalOptions - 1;

        std::vector<GrupoCorUI> titlePalette = {
            {"█", 100, 200, 100},
            {"░", 50, 100, 50},
            {"_", 100, 200, 100},
            {"|", 100, 200, 100},
            {"-", 100, 200, 100}
        };

        auto builder = [&](UIDynamicBox& box, int selA, float optStartX, float optStartY, float pixelScale) {
            float logicalW = UIRenderer2D::LOGICAL_WIDTH;
            float logicalH = UIRenderer2D::LOGICAL_HEIGHT;

            float currentY = 80.0f;
            int interactiveCounter = 0;

            for (const auto& l : lines) {
                std::string plain = l;

                std::string renderText = plain;
                D2D1_COLOR_F color = D2D1::ColorF(0.8f, 0.8f, 0.8f);

                if (plain.length() >= 3 && (plain.substr(0, 3) == " > " || plain.substr(0, 3) == "   ")) {
                    std::string content = plain.substr(3);
                    if (!content.empty() && content.find("BOLSO:") == std::string::npos && content.find("(Nada equipado)") == std::string::npos && content.find("Nenhum item") == std::string::npos) {
                        std::wstring wContent = MenuRaycasterUtils::utf8_to_wstring(content);
                        MenuRaycasterUtils::adicionarOpcaoMenu(box, wContent, 50.0f, currentY, (interactiveCounter == selA), D2D1::ColorF(0.8f, 0.8f, 0.8f), false);
                        interactiveCounter++;
                        currentY += 25.0f;
                        continue;
                    } else {
                        renderText = content;
                        if (content.find("---") != std::string::npos) color = D2D1::ColorF(0.5f, 0.5f, 0.5f);
                    }
                } else {
                    if (plain.find("---") != std::string::npos) color = D2D1::ColorF(0.5f, 0.5f, 0.5f);
                }

                std::wstring wLine = MenuRaycasterUtils::utf8_to_wstring(renderText);
                box.AddText(wLine, 50.0f, currentY, 16.0f, color, false);
                currentY += 25.0f;
            }
            
            currentY += 20.0f;
            box.AddText(L"[ENTER] Selecionar  [ESC] Voltar  [I] Inspecionar  [Q] Equipar  [F] Largar", logicalW / 2.0f, currentY, 14.0f, D2D1::ColorF(0.5f, 0.5f, 0.5f), true);
        };

        auto extraHandler = [&](char key, int& selA) -> bool {
            if (key == 'q' || key == 'Q') {
                if (state != MAIN && selA < (int)realIndices.size() && realIndices[selA] != -1) {
                    Item* itemToEquip = indexToItemMap[realIndices[selA]];
                    InventoryController::useOrEquip(currentPlayer, itemToEquip, false);
                }
                return true;
            }
            if (key == 'i' || key == 'I') {
                if (state != MAIN && selA < (int)realIndices.size() && realIndices[selA] != -1) {
                    Item* itemToInspect = indexToItemMap[realIndices[selA]];
                    MenuRaycasterUtils::renderizarPopupCaixa({}, titlePalette, 1, [&](UIDynamicBox& ibox, int, float lw, float lh){
                        std::wstring wName = MenuRaycasterUtils::utf8_to_wstring(itemToInspect->getItemName());
                        ibox.SetTitle(L"INSPECAO: " + wName, D2D1::ColorF(1.0f, 1.0f, 0.0f));
                        
                        std::vector<std::string> details = itemToInspect->getInspectionDetails(currentPlayer);
                        float textY = 120.0f;
                        for (const auto& det : details) {
                            std::wstring wDet = MenuRaycasterUtils::utf8_to_wstring(det);
                            ibox.AddText(wDet, lw/2.0f, textY, 18.0f, D2D1::ColorF(0.8f, 0.8f, 0.8f), true);
                            textY += 30.0f;
                        }
                        
                        MenuRaycasterUtils::adicionarOpcaoMenu(ibox, L"VOLTAR", lw/2.0f, textY + 40.0f, true, D2D1::ColorF(0.7f, 0.7f, 0.7f), true);
                    }, nullptr);
                }
                return true;
            }
            if (key == 'f' || key == 'F') {
                if (state != MAIN && selA < (int)realIndices.size() && realIndices[selA] != -1) {
                    Item* itemToDrop = indexToItemMap[realIndices[selA]];
                    currentPlayer->getInventory()->removeItem(itemToDrop);
                }
                return true;
            }
            return false;
        };

        std::vector<std::string> emptyArt;
        std::vector<std::string> emptyOptions(totalOptions);
        int res = MenuRaycasterUtils::renderizarMenuPadrao(
            MenuRaycasterUtils::utf8_to_wstring(boxTitle),
            D2D1::ColorF(1.0f, 1.0f, 0.0f),
            emptyOptions,
            emptyArt,
            titlePalette,
            MenuRaycasterUtils::PosicaoArte::NENHUMA,
            1.0f,
            builder,
            extraHandler,
            ArtesInventario::logoInventario,
            {}
        );
        
        *selRef = (res != -1) ? res : totalOptions - 1;
        char key = (res == -1) ? 27 : '\r';

        if (key == 's' || key == 'S') { 
            (*selRef)++;
        } else if (key == '\n' || key == '\r') {
            fullRedrawInv = true;
            if (totalOptions > 0) {
                if (state == MAIN) {
                    int offset = currentPlayer->getQuickConsumable() ? 1 : 0;
                    int logicalChoice = realIndices[*selRef];
                    
                    if (logicalChoice == 7 + offset) {
                        running = false;
                    } else if (offset == 1 && logicalChoice == 0) {
                        // Consumivel rapido
                        Item* quickItem = currentPlayer->getQuickConsumable();
                        std::string quickName = quickItem->getItemName();
                        int countBefore = currentPlayer->getInventory()->countItem(quickName);
                        if (countBefore > 0) {
                            bool turnAlreadyUsed = turnConsumed && *turnConsumed;
                            ItemUsageInfo info = InventoryController::useOrEquip(currentPlayer, quickItem, turnAlreadyUsed);
                            if (turnConsumed && info.turnConsumed) *turnConsumed = true;
                            if (currentPlayer->getItemSelectedForUse() != nullptr) {
                                running = false;
                            }
                            if (currentPlayer->getInventory()->countItem(quickName) == 0) {
                                currentPlayer->unequipConsumable();
                            }
                        } else {
                            currentPlayer->unequipConsumable();
                        }
                        if (turnConsumed && *turnConsumed) running = false;
                        
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    } else {
                        int cat = logicalChoice - offset;
                        if (cat == 0) state = ARSENAL;
                        else if (cat == 1) state = CONSUMABLES;
                        else if (cat == 2) state = STORAGE;
                        else if (cat == 3) state = QUEST;
                        subSelection = 0;
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    }
                } else {
                    int idx = realIndices[*selRef];
                    if (idx == -1) {
                        state = MAIN;
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    } else {
                        Item* foundItem = indexToItemMap[idx];
                        bool isEquippable = foundItem->isEquippable();
                        
                        bool submenuOpen = true;
                        while(submenuOpen) {
                            int subOption = readInventoryPopupSelection(
                                "OPCOES DE ITEM", 
                                "",
                                {"O que deseja fazer com:", std::string(">> ") + foundItem->getItemName() + std::string(" <<")}, 
                                {"Usar / Equipar", "Inspecionar", "[ VOLTAR ]"}
                            );
                            
                            if (subOption == 2) { // VOLTAR
                                submenuOpen = false;
                                if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                                break;
                            } else if (subOption == 0) { // Usar / Equipar
                                bool turnAlreadyUsed = turnConsumed && *turnConsumed;
                                if (isEquippable) {
                                    ItemUsageInfo info = InventoryController::useOrEquip(currentPlayer, foundItem, turnAlreadyUsed);
                                    displayItemResult(info, foundItem, turnConsumed);
                                } else {
                                    int availableCount = currentPlayer->getInventory()->countItem(foundItem->getItemName());
                                    int amountToUse = 1;
                                    
                                    if (availableCount > 1) {
                                        int countChoice = readInventoryPopupSelection(
                                            "QUANTIDADE: " + foundItem->getItemName(),
                                            "",
                                            {"Voce possui " + std::to_string(availableCount) + " unidades deste item."},
                                            {"Usar UMA unidade", "Usar TODAS as unidades", "Usar amount ESPECIFICA", "[ CANCELAR ]"}
                                        );
                                        
                                        if (countChoice == 0) {
                                            amountToUse = 1;
                                        } else if (countChoice == 1) {
                                            amountToUse = availableCount;
                                        } else if (countChoice == 2) {
                                            std::string msgQtd = "Quantidade (1 a " + std::to_string(availableCount) + ", 0 cancelar): ";
                                            amountToUse = readInventoryPopupInteger("QUANTIDADE", msgQtd, 0, availableCount);
                                        } else {
                                            if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                                            continue; 
                                        }
                                    }
                                    
                                    if (amountToUse <= 0) {
                                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                                        continue;
                                    }
                                    
                                    std::string itemName = foundItem->getItemName();
                                    int countBefore = currentPlayer->getInventory()->countItem(itemName);
                                    bool consumedAnyTurn = false;
                                    for (int i = 0; i < amountToUse; ++i) {
                                        Item* currentItem = nullptr;
                                        for (auto* it : currentPlayer->getInventory()->getAllItems()) {
                                            if (it && it->getItemName() == itemName) {
                                                currentItem = it;
                                                break;
                                            }
                                        }
                                        if (!currentItem) break;

                                        turnAlreadyUsed = turnConsumed && *turnConsumed;
                                        ItemUsageInfo info = InventoryController::useOrEquip(currentPlayer, currentItem, turnAlreadyUsed);
                                        if (info.turnConsumed) consumedAnyTurn = true;
                                        
                                        if (currentPlayer->getItemSelectedForUse() != nullptr) {
                                            if (amountToUse > 1) {
                                                displayInventoryPopupMessage("SISTEMA", {"Este item requer selecao de alvo e", "sera usado apenas uma vez."});
                                            }
                                            break;
                                        }
                                        
                                        int countAfter = currentPlayer->getInventory()->countItem(itemName);
                                        if (countAfter == countBefore && !isEquippable) {
                                            break;
                                        }
                                    }
                                    
                                    if (currentPlayer->getQuickConsumable() && currentPlayer->getInventory()->countItem(currentPlayer->getQuickConsumable()->getItemName()) == 0) {
                                        currentPlayer->unequipConsumable();
                                    }

                                    if (turnConsumed && consumedAnyTurn) {
                                        *turnConsumed = true;
                                    }
                                }
                                submenuOpen = false; 
                                if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                            } else if (subOption == 1) { // Inspecionar
                                std::vector<std::string> details = foundItem->getInspectionDetails(currentPlayer);
                                std::vector<std::string> inspLines;
                                inspLines.push_back(std::string(" >> ") + foundItem->getItemName() + std::string(" <<"));
                                inspLines.push_back("");
                                inspLines.insert(inspLines.end(), details.begin(), details.end());
                                
                                displayInventoryPopupMessage("INSPECAO DE ITEM", inspLines);
                                if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                            }
                        }
                        
                        if (turnConsumed && *turnConsumed) running = false;
                        if (is3D) RaycasterFrame::restaurarUltimoQuadro();
                    }
                }
            }
        }
    }
}


