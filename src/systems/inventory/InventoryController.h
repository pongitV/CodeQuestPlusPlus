#pragma once
#include <string>

class Character;
class Item;

enum class ItemResult {
    Equipped,
    Unequipped,
    Used_Turn,
    Used_NoTurn,
    Error_TurnAlreadyUsed,
    Error_BrokenShield,
    Error_Requirements,
    Error_CannotUse,
    None,

    // Aliases legados
    Equipou = Equipped,
    Desequipou = Unequipped,
    Usou_Turno = Used_Turn,
    Usou_SemTurno = Used_NoTurn,
    Erro_TurnoJaUsado = Error_TurnAlreadyUsed,
    Erro_EscudoQuebrado = Error_BrokenShield,
    Erro_Requisitos = Error_Requirements,
    Erro_NaoPodeUsar = Error_CannotUse,
    Nada = None
};

using ResultadoItem = ItemResult;

struct ItemUsageInfo {
    ItemResult result;
    std::string itemName;
    std::string extraMessage;
    bool turnConsumed = false;

    // Aliases de campos legados
    ItemResult resultado() const { return result; }
};

using UsoItemInfo = ItemUsageInfo;

class InventoryController {
public:
    static ItemUsageInfo useOrEquip(Character* player, Item* item, bool turnAlreadyConsumed);
    static std::string getErrorMessage(Item* item, bool inCombat);

    // Compatibilidade legada
    static ItemUsageInfo usarOuEquipar(Character* jogador, Item* item, bool turnoJaFoiConsumido) {
        return useOrEquip(jogador, item, turnoJaFoiConsumido);
    }
    static std::string obterMensagemErro(Item* item, bool emCombate) {
        return getErrorMessage(item, emCombate);
    }
};

using ControleInventario = InventoryController;
