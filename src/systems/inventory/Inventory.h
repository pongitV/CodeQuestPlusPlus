#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>

#include "Item.h"

// ItemIndex armazena os metadados de itens ordenados para exibicao do inventario.
struct ItemIndex {
    size_t index = 0;
    std::string name = "";
    EquipmentType type = EquipmentType::None;
    bool isEquipped = false;
};

// ItemCountCache armazena a contagem de itens acumulados para busca rapida.
struct ItemCountCache {
    std::unordered_map<std::string, int> count;
    bool dirty = true;

    void invalidate() { dirty = true; }
    void invalidar() { invalidate(); }
};

// Inventory gerencia a colecao de itens, moedas de ouro e contagem de itens do personagem.
class Inventory
{
private:
    std::vector<std::unique_ptr<Item>> itemList;
    std::unordered_map<std::string, int> itemCountMap_;
    mutable ItemCountCache countCache_;
    int goldAmount;

public:
    std::vector<Item*> getAllItems() const { 
        std::vector<Item*> rawItems;
        rawItems.reserve(itemList.size());
        std::transform(itemList.begin(), itemList.end(), std::back_inserter(rawItems), [](const auto& item) { return item.get(); });
        return rawItems;
    }
    std::vector<Item*> obterTodosOsItens() const { return getAllItems(); }
    
    std::vector<ItemIndex> getSortedIndices() const;
    std::vector<ItemIndex> obterIndicesOrdenados() const { return getSortedIndices(); }
    
    Inventory();
    ~Inventory() = default;

    // Funcoes de estado e consulta
    bool isEmpty() const;
    bool estaVazio() const { return isEmpty(); }

    int getGold() const;
    int obterOuro() const { return getGold(); }

    int countItem(const std::string& itemName) const;
    int contarItem(const std::string& nomeDoItem) const { return countItem(nomeDoItem); }
    int getItemCount(const std::string& itemName) const { return countItem(itemName); }
    int obterQuantidadeItem(const std::string& nomeDoItem) const { return countItem(nomeDoItem); }

    // Manipulacao do inventario
    void addGold(int additionalAmount);
    void adicionarOuro(int quantidadeAdicional) { addGold(quantidadeAdicional); }

    void addItem(std::unique_ptr<Item> newItem);
    void adicionarItem(std::unique_ptr<Item> novoItem) { addItem(std::move(novoItem)); }

    void removeItem(const std::string& itemName);
    void removerItem(const std::string& nomeDoItem) { removeItem(nomeDoItem); }

    void removeItem(Item* exactItem);
    void removerItem(Item* itemExato) { removeItem(itemExato); }

    // Busca e interacao com itens
    Item* findItemByCode(const std::string& enteredCode, Item* equippedWeapon, Item* equippedShield, Item* equippedArmor);
    Item* buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada) {
        return findItemByCode(codigoDigitado, armaEquipada, escudoEquipado, armaduraEquipada);
    }
};

using Mochila = Inventory;
