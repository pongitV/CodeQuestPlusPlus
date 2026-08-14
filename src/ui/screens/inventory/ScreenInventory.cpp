#include <map>

#include "ScreenInventory.h"
#include "../../UIManager.h"
#include "../../../systems/inventory/Item.h"
#include "../../../core/utils/StringBuffer.h"

void TelaInventario::displayCabecalhoInventario(bool animar, int startY) {
    GerenciadorPerspectiva::obterInventarioUI().displayCabecalho(animar, startY);
}

void TelaInventario::displayCaixaEquipados(Character* currentPlayer) {
    GerenciadorPerspectiva::obterInventarioUI().displayCaixaEquipados(currentPlayer);
}

std::vector<std::pair<std::string, Item*>> TelaInventario::obterListaCategoria(Character* currentPlayer, int categoria, bool mostrarPrecos)
{
    std::vector<std::pair<std::string, Item*>> lista;
    if (!currentPlayer) return lista;

    std::map<std::string, std::vector<Item*>> itensAgrupados;

    for (Item* item : currentPlayer->obterInventario()->obterTodosOsItens()) {
        EquipmentType tipo = item->getType();
        if (categoria == 0 && (tipo == EquipmentType::Weapon || tipo == EquipmentType::Shield || tipo == EquipmentType::Armor)) {
            itensAgrupados[item->getNameItem() + item->obterInfoStatus()].push_back(item);
        } else if (categoria == 1 && tipo == EquipmentType::Consumable) {
            itensAgrupados[item->getNameItem()].push_back(item);
        } else if (categoria == 2 && tipo == EquipmentType::Material) {
            itensAgrupados[item->getNameItem()].push_back(item);
        } else if (categoria == 3 && tipo == EquipmentType::Quest) {
            itensAgrupados[item->getNameItem()].push_back(item);
        }
    }

    std::string sufixo = (categoria == 1 || categoria == 2) ? "G / un" : "G";
    if (categoria == 3) sufixo = "";

    StringBuffer sbuf(4);

    for (auto const& [nome, itensGrupo] : itensAgrupados) {
        Item* item = itensGrupo.front();
        sbuf.Clear();
        if (itensGrupo.size() > 1) {
            sbuf.Append(static_cast<int>(itensGrupo.size()));
            sbuf.Append("x ");
        }

        bool algumEquipado = false;
        for (Item* it : itensGrupo) {
            if (currentPlayer->isItemEquipado(it)) {
                algumEquipado = true;
                break;
            }
        }

        if (algumEquipado) {
            sbuf.Append("[E] ");
        }
        sbuf.Append(nome);

        if (mostrarPrecos) {
            sbuf.Append(" (Venda: ");
            sbuf.Append(item->obterPrecoVenda());
            sbuf.Append(sufixo);
            sbuf.Append(")");
        }

        auto vec = sbuf.ToVector();
        std::string res;
        for (const auto& part : vec) res += part;

        lista.push_back({res, item});
    }
    return lista;
}

void TelaInventario::displayInspecaoItem(Item* item, Character* currentPlayer)
{
    GerenciadorPerspectiva::obterInventarioUI().displayDetalheItem(item);
}
