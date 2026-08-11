#pragma once

#include <string>
#include <vector>
#include "../../../../ui/interfaces/IInventoryUI.h"

class Character;
class Item;

class TelaInventarioRaycaster : public IInventarioUI {
public:
    void renderizarMenu(const std::vector<std::string>& linhas, const std::string& titulo, int selecaoAtual, int& outW, int& outH) override;
    void displayCabecalho(bool ehIde, int startY) override;
    void displayCaixaEquipados(Character* currentPlayer) override;
    void displayDetalheItem(Item* item) override;
};
