#include "InventoryController.h"
#include "Item.h"
#include "./equipment/WeaponEquipment.h"
#include "./equipment/ShieldEquipment.h"
#include "./equipment/ArmorEquipment.h"
#include "../../entities/character/Character.h"
#include <string>

UsoItemInfo ControleInventario::usarOuEquipar(Character* jogador, Item* item, bool turnoJaFoiConsumido) {
    if (turnoJaFoiConsumido) {
        return {ResultadoItem::Erro_TurnoJaUsado, "", "", false};
    }

    if (item->isEquipavel()) {
        if (item->obterTipo() == TipoEquipamento::ESCUDO && item->obterDurabilidadeAtualEscudo() <= 0) {
            return {ResultadoItem::Erro_EscudoQuebrado, item->getNameItem(), "", false};
        }

        bool desequipou = false;
        if (item == jogador->obterArma()) {
            jogador->desequiparArma();
            desequipou = true;
        } else if (item == jogador->obterEscudo()) {
            jogador->desequiparEscudo();
            desequipou = true;
        } else if (item == jogador->obterArmadura()) {
            jogador->desequiparArmadura();
            desequipou = true;
        }

        if (desequipou) {
            return {ResultadoItem::Desequipou, item->getNameItem(), "", true};
        }

        if (!item->podeSerEquipadoPor(jogador)) {
            return {ResultadoItem::Erro_Requisitos, item->getNameItem(), item->obterMensagemRequisito(), false};
        }

        jogador->equiparItem(item);
        return {ResultadoItem::Equipou, item->getNameItem(), "", true};
    }

    bool consumiu = false;
    if (item->usarDoInventario(jogador, &consumiu)) {
        return {ResultadoItem::Usou_Turno, item->getNameItem(), "", consumiu};
    }

    return {ResultadoItem::Erro_NaoPodeUsar, item->getNameItem(), "", false};
}

std::string ControleInventario::obterMensagemErro(Item* item, bool emCombate) {
    switch (item->obterTipo()) {
        case TipoEquipamento::MATERIAL:
            return "Materiais sao utilizados para NPCs especializados.";
        case TipoEquipamento::MISSAO:
            return "Itens de missao sao ativados automaticamente.";
        case TipoEquipamento::CONSUMIVEL:
            return "Este consumivel nao pode ser usado " + std::string(emCombate ? "no combat!" : "fora de combat!");
        default:
            return "Este item nao possui uso direto no inventory.";
    }
}
