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
    std::string nome = "";
    TipoEquipamento tipo = TipoEquipamento::NENHUM;
    bool isEquipado = false;
};

// ItemCountCache armazena a contagem de itens acumulados para busca rapida.
struct ItemCountCache {
    std::unordered_map<std::string, int> contagem;
    bool sujo = true;

    void invalidar() { sujo = true; }
};

// Inventory gerencia a colecao de itens, moedas de ouro e contagem de itens do personagem.
class Inventory
{
private:
    std::vector<std::unique_ptr<Item>> listaDeItens;
    std::unordered_map<std::string, int> contagemItens_;
    mutable ItemCountCache countCache_;
    int quantidadeDeOuro;

public:
    std::vector<Item*> obterTodosOsItens() const { 
        std::vector<Item*> itensCrus;
        itensCrus.reserve(listaDeItens.size());
        std::transform(listaDeItens.begin(), listaDeItens.end(), std::back_inserter(itensCrus), [](const auto& item) { return item.get(); });
        return itensCrus;
    }
    
    std::vector<ItemIndex> obterIndicesOrdenados() const;
    
    Inventory();
    ~Inventory() = default;

    // Funcoes de estado e consulta
    bool estaVazio() const;
    int obterOuro() const;
    int contarItem(const std::string& nomeDoItem) const;
    int obterQuantidadeItem(const std::string& nomeDoItem) const { return contarItem(nomeDoItem); }

    // Manipulacao do inventario
    void adicionarOuro(int quantidadeAdicional);
    void adicionarItem(std::unique_ptr<Item> novoItem);
    void removerItem(const std::string& nomeDoItem);
    void removerItem(Item* itemExato);

    // Busca e interacao com itens
    Item* buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada);

    bool isEmpty() const { return estaVazio(); }
    int getGold() const { return obterOuro(); }
    int getItemCount(const std::string& itemName) const { return contarItem(itemName); }
    void addGold(int amount) { adicionarOuro(amount); }
    void addItem(std::unique_ptr<Item> item) { adicionarItem(std::move(item)); }
    void removeItem(const std::string& name) { removerItem(name); }
    void removeItem(Item* item) { removerItem(item); }
    std::vector<Item*> getAllItems() const { return obterTodosOsItens(); }
};

using Mochila = Inventory;
