#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Item.h"

class ItemFactory {
public:
    // Cria um item de forma type-safe baseada num Enum.
    static std::unique_ptr<Item> createItem(ItemID id);

    // Mantido para retrocompatibilidade com sistema de Saves e Encantamentos (+).
    static std::unique_ptr<Item> createItem(const std::string& name);
    
    static std::vector<std::unique_ptr<Item>> createMultipleItems(ItemID id, int amount);
    static std::vector<std::unique_ptr<Item>> createPotionKit(int amount = 3);

    static std::string getNameFromID(ItemID id);
    static ItemID getIDFromName(const std::string& name);

    // Metodos legados em portugues
    static std::unique_ptr<Item> criarItem(ItemID id) { return createItem(id); }
    static std::unique_ptr<Item> criarItem(const std::string& nome) { return createItem(nome); }
    static std::vector<std::unique_ptr<Item>> criarVariosItens(ItemID id, int amount) { return createMultipleItems(id, amount); }
    static std::vector<std::unique_ptr<Item>> criarKitPocoes(int amount = 3) { return createPotionKit(amount); }
    static std::string getNameDeID(ItemID id) { return getNameFromID(id); }
    static ItemID obterIDDeNome(const std::string& nome) { return getIDFromName(nome); }
};
