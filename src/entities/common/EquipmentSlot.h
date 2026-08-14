#pragma once

enum class EquipmentSlot {
    MainHand,
    OffHand,
    Armor,
    Accessory1,
    Consumable,

    // Aliases para compatibilidade legada
    MAO_PRINCIPAL = MainHand,
    MAO_SECUNDARIA = OffHand,
    ARMADURA = Armor,
    ACESSORIO_1 = Accessory1,
    CONSUMIVEL = Consumable
};

using SlotEquipamento = EquipmentSlot;
