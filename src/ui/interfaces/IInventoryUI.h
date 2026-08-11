#pragma once
#include <string>
#include <vector>
class Character;
class Item;

class IInventarioUI {
public:
    virtual ~IInventarioUI() = default;
    virtual void displayCabecalho(bool animar, int startY) = 0;
    virtual void displayCaixaEquipados(Character* jogador) = 0;
    virtual void displayDetalheItem(Item* item) = 0;
    virtual void renderizarMenu(const std::vector<std::string>& linhas, const std::string& titulo, int selecaoAtual, int& outW, int& outH) = 0;
};
