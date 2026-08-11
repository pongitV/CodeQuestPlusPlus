#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Item.h"

class ItemFactory {
public:

    // Cria um item de forma type-safe baseada num Enum.
    static std::unique_ptr<Item> criarItem(ItemID id);

    // Mantido para retrocompatibilidade com system de Saves e Encantamentos (+).
    static std::unique_ptr<Item> criarItem(const std::string& nome);
    
    static std::vector<std::unique_ptr<Item>> criarVariosItens(ItemID id, int amount);
    static std::vector<std::unique_ptr<Item>> criarKitPocoes(int amount = 3);

    static std::string getNameDeID(ItemID id);
    static ItemID obterIDDeNome(const std::string& nome);

    // English Aliases
    static std::unique_ptr<Item> createItem(ItemID id) { return criarItem(id); }
    static std::unique_ptr<Item> createItem(const std::string& name) { return criarItem(name); }
    static std::vector<std::unique_ptr<Item>> createMultipleItems(ItemID id, int amount) { return criarVariosItens(id, amount); }
    static std::string getNameFromID(ItemID id) { return getNameDeID(id); }
    static ItemID getIDFromName(const std::string& name) { return obterIDDeNome(name); }
};

using ItemFactory = ItemFactory;
