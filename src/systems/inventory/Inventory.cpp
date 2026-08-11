#include <algorithm>
#include <unordered_map>

#include "Inventory.h"
#include "Item.h"
#include "./items/ConsumableItem.h"
#include "../../core/state/GameMenu.h"
Inventory::Inventory() : quantidadeDeOuro(0) {}

bool Inventory::estaVazio() const { return listaDeItens.empty(); }

int Inventory::obterOuro() const { return quantidadeDeOuro; }

int Inventory::contarItem(const std::string& nomeDoItem) const 
{
    auto it = contagemItens_.find(nomeDoItem);
    return it != contagemItens_.end() ? it->second : 0;
}


void Inventory::adicionarOuro(int quantidadeAdicional) 
{ 
    quantidadeDeOuro = std::max(0, quantidadeDeOuro + quantidadeAdicional); 
}

std::vector<ItemIndex> Inventory::obterIndicesOrdenados() const {
    std::vector<ItemIndex> indices;
    indices.reserve(listaDeItens.size());
    for (size_t i = 0; i < listaDeItens.size(); ++i) {
        if (listaDeItens[i]) {
            indices.push_back({i, listaDeItens[i]->getNameItem(), TipoEquipamento::NENHUM, false});
        }
    }
    std::sort(indices.begin(), indices.end(), [](const ItemIndex& a, const ItemIndex& b) {
        return a.nome < b.nome;
    });
    return indices;
}

void Inventory::adicionarItem(std::unique_ptr<Item> novoItem) 
{ 
    if (novoItem) {
        contagemItens_[novoItem->getNameItem()]++;
        countCache_.invalidar();
        listaDeItens.push_back(std::move(novoItem));
    }
}

namespace {
    void decrementarContagemEErapar(std::vector<std::unique_ptr<Item>>& lista, std::unordered_map<std::string, int>& mapContagem, std::vector<std::unique_ptr<Item>>::iterator it) {
        std::string nome = (*it)->getNameItem();
        auto mapIt = mapContagem.find(nome);
        if (mapIt != mapContagem.end()) {
            if (--mapIt->second <= 0) {
                mapContagem.erase(mapIt);
            }
        }
        lista.erase(it);
    }
}

void Inventory::removerItem(const std::string& nomeDoItem) 
{
    auto it = std::find_if(listaDeItens.begin(), listaDeItens.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item->getNameItem() == nomeDoItem;
    });
    
    if (it != listaDeItens.end()) 
    {
        decrementarContagemEErapar(listaDeItens, contagemItens_, it);
    }
}

void Inventory::removerItem(Item* itemExato) 
{
    if (!itemExato) return;
    auto it = std::find_if(listaDeItens.begin(), listaDeItens.end(), [&](const std::unique_ptr<Item>& item) 
    {
        return item.get() == itemExato;
    });
    
    if (it != listaDeItens.end()) {
        decrementarContagemEErapar(listaDeItens, contagemItens_, it);
    }
}

Item* Inventory::buscarItemPorCodigo(const std::string& codigoDigitado, Item* armaEquipada, Item* escudoEquipado, Item* armaduraEquipada)
{
    if (codigoDigitado.length() < 2) return nullptr;

    char letraDaCategoria = std::toupper(codigoDigitado.back());
    std::string parteNumerica = codigoDigitado.substr(0, codigoDigitado.length() - 1);
    
    if (!std::all_of(parteNumerica.begin(), parteNumerica.end(), ::isdigit)) return nullptr;
    
    int indiceDoItem = std::stoi(parteNumerica);
    if (indiceDoItem <= 0) return nullptr;

    if (letraDaCategoria == 'E')
    {
        if (indiceDoItem == 1) return armaEquipada;
        if (indiceDoItem == 2) return escudoEquipado;
        if (indiceDoItem == 3) return armaduraEquipada;
        return nullptr;
    }

    auto buscarPorTipoAgrupado = [&](auto condicao) -> Item* {
        std::unordered_map<std::string, size_t> cacheDeIndice;
        std::vector<std::string> nomesDosItensExibidos;

        for (size_t i = 0; i < listaDeItens.size(); ++i) {
            Item* itemAtual = listaDeItens[i].get();
            if (condicao(itemAtual)) {
                if (cacheDeIndice.find(itemAtual->getNameItem()) == cacheDeIndice.end()) {
                    cacheDeIndice[itemAtual->getNameItem()] = i;
                    nomesDosItensExibidos.push_back(itemAtual->getNameItem());
                }
            }
        }
        

        if (indiceDoItem > 0 && indiceDoItem <= static_cast<int>(nomesDosItensExibidos.size())) {
            size_t indiceOriginalNoInventario = cacheDeIndice[nomesDosItensExibidos[indiceDoItem - 1]];
            return listaDeItens[indiceOriginalNoInventario].get();
        }
        return nullptr;
    };

    switch (letraDaCategoria) {
        case 'A':
            return buscarPorTipoAgrupado([&](Item* itemAvaliado) { 
                return itemAvaliado->isEquipavel() && itemAvaliado != armaEquipada && itemAvaliado != escudoEquipado && itemAvaliado != armaduraEquipada; 
            });
        case 'C': return buscarPorTipoAgrupado([](Item* itemAvaliado) { return itemAvaliado->obterTipo() == TipoEquipamento::CONSUMIVEL; });
        case 'S': return buscarPorTipoAgrupado([](Item* itemAvaliado) { return itemAvaliado->obterTipo() == TipoEquipamento::MATERIAL; });
        case 'M': return buscarPorTipoAgrupado([](Item* itemAvaliado) { return itemAvaliado->obterTipo() == TipoEquipamento::MISSAO; });
        default:  return nullptr;
    }
}
