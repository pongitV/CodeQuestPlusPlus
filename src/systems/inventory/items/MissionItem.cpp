#include "MissionItem.h"
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"

MissionItem::MissionItem(const std::string& nome, int price) : Item(price), nome(nome) {}

std::string MissionItem::getNameItem() const { return nome; }
TipoEquipamento MissionItem::obterTipo() const { return TipoEquipamento::MISSAO; }

std::vector<std::string> MissionItem::obterDetalhesInspecao(Character* /*character*/) const {
    std::vector<std::string> detalhes;
    detalhes.push_back(" > Tipo: Item de Missao");
    if (!descricaoInspecao.empty()) {
        for (const auto& desc : descricaoInspecao) detalhes.push_back(desc);
    } else {
        detalhes.push_back(" > Lore: Um item misterioso e importante para sua jornada.");
    }
    return detalhes;
}

std::unique_ptr<Item> fabricarMissionItem(ItemID id) {
    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::DispositivoLinguagem, []() { 
            auto i = std::make_unique<MissionItem>(ItemFactory::getNameDeID(ItemID::DispositivoLinguagem), 500); 
            i->definirDescricaoInspecao({" > Lore: Um estranho artefato de plastico com teclas.", "   Nao parece pertencer a este mundo, mas emana", "   uma energia peculiar..."});
            return i;
        }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
