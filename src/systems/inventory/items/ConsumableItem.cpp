#include "ConsumableItem.h"
#include "../../../core/state/Status.h"
#include "../../../entities/character/Character.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../ui/UIManager.h"
#include "../../../core/utils/InputControl.h"
#include <functional>
#include <unordered_map>
#include "../ItemFactory.h"
#include "../../../core/utils/Color.h"

static void mostrarAvisoConsumivel(const std::string& msg, Color corMsg = Color::WHITE) {
    (void)corMsg;
    TelaCombate::adicionarMensagemFixa(msg);
}


ItemConsumivel::ItemConsumivel(const std::string& nome, int price) : Item(price), nome(nome)
{
}

std::string ItemConsumivel::getNameItem() const { return nome; }

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }

std::vector<std::string> ItemConsumivel::obterDetalhesInspecao(Character* /*character*/) const {
    std::vector<std::string> detalhes;
    detalhes.push_back(" > Tipo: Consumable");
    if (!descricaoInspecao.empty()) {
        for (const auto& desc : descricaoInspecao) detalhes.push_back(" > Efeitos: " + desc);
    } else {
        detalhes.push_back(" > Efeitos: Pode ser consumido para aplicar efeitos.");
    }
    return detalhes;
}

std::unique_ptr<Item> fabricarItemConsumivel(ItemID id) {
    auto criarPocaoCura = []() {
        auto cura = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::PocaoCura30), 6);
        cura->adicionarPropriedade(Propriedade::ConsumivelCura);
        cura->definirDescricaoInspecao("Restaura 30% da sua Health Maxima.");
        cura->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (usuario->obterVida() >= usuario->obterVidaMaxima()) {
                mostrarAvisoConsumivel("Sua health ja esta cheia!");
                return true;
            }
            int vidaAntes = usuario->obterVida();
            int curaEstimada = static_cast<int>(usuario->obterVidaMaxima() * 0.30);
            usuario->modificarVida(curaEstimada);
            int vidaDepois = usuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            mostrarAvisoConsumivel(item->getNameItem() + " usada! +" + std::to_string(curaReal) + " HP. (Health atual: " + std::to_string(vidaDepois) + "/" + std::to_string(usuario->obterVidaMaxima()) + ")", Color::GREEN);
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
                std::string nomeDesteItem = item->getNameItem();
                for (auto* outroItem : usuario->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != item && outroItem->getNameItem() == nomeDesteItem) {
                        usuario->equiparItem(outroItem);
                        break;
                    }
                }
            }

            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return cura;
    };

    auto criarTalisma = [](ItemID id, Propriedade prop, TipoAtributo buffAtr, TipoAtributo debuffAtr, const std::string& desc) {
        auto t = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(id), 120);
        t->adicionarPropriedade(prop);
        t->definirDescricaoInspecao(desc);
        t->definirAcaoInventario([buffAtr, debuffAtr](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            usuario->alterarAtributoEstatico(buffAtr, 5);
            usuario->alterarAtributoEstatico(debuffAtr, -5);
            mostrarAvisoConsumivel(item->getNameItem() + " consumido!");
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
                std::string nomeDesteItem = item->getNameItem();
                for (auto* outroItem : usuario->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != item && outroItem->getNameItem() == nomeDesteItem) {
                        usuario->equiparItem(outroItem);
                        break;
                    }
                }
            }

            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return t;
    };

    auto criarBuffAtributos = [](ItemID id) {
        auto buff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(id), 3);
        buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
        buff->definirDescricaoInspecao("Aumenta seus attributes em 1.5x por 2 turnos.");
        buff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { mostrarAvisoConsumivel("Pocoes de buff so podem ser usadas em combat!"); return true; }
            usuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2));
            usuario->definirMultiplicador(1.5);
            mostrarAvisoConsumivel(item->getNameItem() + " consumida! Attributes ampliados em 1.5x por 2 turnos!", Color::GREEN_CLARO);
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
                std::string nomeDesteItem = item->getNameItem();
                for (auto* outroItem : usuario->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != item && outroItem->getNameItem() == nomeDesteItem) {
                        usuario->equiparItem(outroItem);
                        break;
                    }
                }
            }

            usuario->obterInventario()->removerItem(item);
            *turnoFoiConsumido = true;
            return true;
        });
        return buff;
    };

    auto criarFoodMerchant = [](ItemID id, int curaHP, int price, const std::string& desc) {
        auto comida = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(id), price);
        comida->adicionarPropriedade(Propriedade::ConsumivelCura);
        comida->definirDescricaoInspecao(desc);
        comida->definirAcaoInventario([curaHP](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (usuario->obterVida() >= usuario->obterVidaMaxima()) {
                mostrarAvisoConsumivel("Sua health ja esta cheia!");
                return true;
            }
            int vidaAntes = usuario->obterVida();
            usuario->modificarVida(curaHP);
            int vidaDepois = usuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            mostrarAvisoConsumivel(item->getNameItem() + " consumido(a)! +" + std::to_string(curaReal) + " HP. (Health atual: " + std::to_string(vidaDepois) + "/" + std::to_string(usuario->obterVidaMaxima()) + ")", Color::GREEN);
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
            }
            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return comida;
    };

    auto criarPocaoCuraGrande = []() {
        auto cura = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::PocaoCuraGrande), 30);
        cura->adicionarPropriedade(Propriedade::ConsumivelCura);
        cura->definirDescricaoInspecao("Restaura 50% da sua Health Maxima.");
        cura->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (usuario->obterVida() >= usuario->obterVidaMaxima()) {
                mostrarAvisoConsumivel("Sua health ja esta cheia!");
                return true;
            }
            int vidaAntes = usuario->obterVida();
            int curaEstimada = static_cast<int>(usuario->obterVidaMaxima() * 0.50);
            usuario->modificarVida(curaEstimada);
            int vidaDepois = usuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            mostrarAvisoConsumivel(item->getNameItem() + " usada! +" + std::to_string(curaReal) + " HP. (Health atual: " + std::to_string(vidaDepois) + "/" + std::to_string(usuario->obterVidaMaxima()) + ")", Color::GREEN);
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
            }
            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return cura;
    };

    auto criarPocaoForcaAlquimica = []() {
        auto buff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::PocaoForcaAlquimica), 40);
        buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
        buff->definirDescricaoInspecao("Aumenta Forca em +5 e Destreza em +3 por 3 turnos em combat.");
        buff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { mostrarAvisoConsumivel("Pocoes de buff so podem ser usadas em combat!"); return true; }
            usuario->adicionarEfeito(std::make_unique<EfeitoGritoGuerra>(3, 5, 3));
            mostrarAvisoConsumivel(item->getNameItem() + " consumida! +5 Forca e +3 Destreza por 3 turnos!", Color::GREEN_CLARO);
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
            }
            usuario->obterInventario()->removerItem(item);
            *turnoFoiConsumido = true;
            return true;
        });
        return buff;
    };

    auto criarPocaoVenenoAlquimica = []() {
        auto debuff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::PocaoVenenoAlquimica), 35);
        debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
        debuff->definirDescricaoInspecao("Aplica Necrose no alvo por 3 turnos (causa 12 de damage por turno).");
        debuff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { mostrarAvisoConsumivel("Pocoes de arremesso so podem ser usadas em combat!"); return true; }
            usuario->definirItemSelecionadoParaUso(item);
            return true;
        });
        debuff->definirAcaoUsar([](Character* /*usuario*/, Character* alvo) {
            if (!Character::isValido(alvo) || alvo->obterVida() <= 0) return;
            alvo->adicionarEfeito(std::make_unique<EfeitoNecrose>(3, 12));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce arremessou a pocao! ") + alvo->getName() + " sofreu necrose (12 damage/turno) por 3 turnos!" + std::string("\n"));
        });
        return debuff;
    };

    auto criarPocaoLentidaoAlquimica = []() {
        auto debuff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::PocaoLentidaoAlquimica), 35);
        debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
        debuff->definirDescricaoInspecao("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
        debuff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { mostrarAvisoConsumivel("Pocoes de arremesso so podem ser usadas em combat!"); return true; }
            usuario->definirItemSelecionadoParaUso(item);
            return true;
        });
        debuff->definirAcaoUsar([](Character* /*usuario*/, Character* alvo) {
            if (!Character::isValido(alvo) || alvo->obterVida() <= 0) return;
            alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce arremessou a pocao! ") + alvo->getName() + " esta sob efeito de Lentidao por 3 turnos!" + std::string("\n"));
        });
        return debuff;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::Maca, [criarFoodMerchant]() { return criarFoodMerchant(ItemID::Maca, 15, 5, "Restaura 15 HP fixo."); }},
        {ItemID::Pao, [criarFoodMerchant]() { return criarFoodMerchant(ItemID::Pao, 25, 10, "Restaura 25 HP fixo."); }},
        {ItemID::Queijo, [criarFoodMerchant]() { return criarFoodMerchant(ItemID::Queijo, 40, 18, "Restaura 40 HP fixo."); }},
        {ItemID::CarneSeca, [criarFoodMerchant]() { return criarFoodMerchant(ItemID::CarneSeca, 60, 30, "Restaura 60 HP fixo."); }},
        {ItemID::PocaoCuraGrande, criarPocaoCuraGrande},
        {ItemID::PocaoForcaAlquimica, criarPocaoForcaAlquimica},
        {ItemID::PocaoVenenoAlquimica, criarPocaoVenenoAlquimica},
        {ItemID::PocaoLentidaoAlquimica, criarPocaoLentidaoAlquimica},
        {ItemID::PocaoCura30, criarPocaoCura},
        {ItemID::PocaoFuria, [criarBuffAtributos]() { return criarBuffAtributos(ItemID::PocaoFuria); }},
        {ItemID::ElixirArcano, [criarBuffAtributos]() { return criarBuffAtributos(ItemID::ElixirArcano); }},
        {ItemID::FrascoGosma, []() {
            auto debuff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::FrascoGosma));
            debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
            debuff->definirDescricaoInspecao("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
            debuff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
                if (!turnoFoiConsumido) { mostrarAvisoConsumivel("Frascos de debuff so podem ser usados em combat!"); return true; }
                usuario->definirItemSelecionadoParaUso(item);
                return true;
            });
            debuff->definirAcaoUsar([](Character* /*usuario*/, Character* alvo) {
                if (!Character::isValido(alvo) || alvo->obterVida() <= 0) return;
                alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce jogou o frasco! ") + alvo->getName() + " esta com lentidao por 3 turnos!" + std::string("\n"));
            });
            return debuff;
        }},
        {ItemID::FrascoFraqueza, []() {
            auto debuff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::FrascoFraqueza));
            debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
            debuff->definirDescricaoInspecao("Aplica Fraqueza no alvo por 3 turnos (-25% Forca).");
            debuff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
                if (!turnoFoiConsumido) { mostrarAvisoConsumivel("Frascos de debuff so podem ser usados em combat!"); return true; }
                usuario->definirItemSelecionadoParaUso(item);
                return true;
            });
            debuff->definirAcaoUsar([](Character* /*usuario*/, Character* alvo) {
                if (!Character::isValido(alvo) || alvo->obterVida() <= 0) return;
                alvo->adicionarEfeito(std::make_unique<EfeitoFraqueza>(3));
            TelaCombate::adicionarMensagemFixa("\n" + TelaCombate::margemCombate() + std::string(">> Voce jogou o frasco! ") + alvo->getName() + " teve sua strength reduzida em 25% por 3 turnos!" + std::string("\n"));
            });
            return debuff;
        }},
        {ItemID::OrgaoRegenerador, []() { 
            auto buff = std::make_unique<ItemConsumivel>(ItemFactory::getNameDeID(ItemID::OrgaoRegenerador), 500); 
            buff->adicionarPropriedade(Propriedade::ConsumivelPoderTroll); 
            buff->definirDescricaoInspecao("Concede a regeneracao do Troll permanentemente (cura 100% HP apos batalhas).");
            buff->definirAcaoInventario([](Item* item, Character* usuario, bool* turnoFoiConsumido) {
                if (usuario->possuiRegeneracaoTroll()) {
                    mostrarAvisoConsumivel("O poder regenerador do Troll ja corre em suas veias!");
                } else {
                    usuario->desbloquearRegeneracaoTroll();
                    usuario->modificarVida(usuario->obterVidaMaxima());
                    mostrarAvisoConsumivel(item->getNameItem() + " consumido! Voce agora heala 100% do seu HP apos cada combat!", Color::GREEN);
                    
                    if (usuario->obterConsumivelRapido() == item) usuario->desequiparConsumivel();
                    
                    usuario->obterInventario()->removerItem(item);
                }
                if (turnoFoiConsumido) *turnoFoiConsumido = true;
                return true;
            });
            return buff; 
        }},
        {ItemID::TalismaUrso, [criarTalisma]() { return criarTalisma(ItemID::TalismaUrso, Propriedade::TalismaForca, TipoAtributo::Forca, TipoAtributo::Inteligencia, "Concede +5 Forca e -5 Inteligencia permanentemente."); }},
        {ItemID::TalismaCorvo, [criarTalisma]() { return criarTalisma(ItemID::TalismaCorvo, Propriedade::TalismaInteligencia, TipoAtributo::Inteligencia, TipoAtributo::Forca, "Concede +5 Inteligencia e -5 Forca permanentemente."); }},
        {ItemID::TalismaLeopardo, [criarTalisma]() { return criarTalisma(ItemID::TalismaLeopardo, Propriedade::TalismaDestreza, TipoAtributo::Destreza, TipoAtributo::Sabedoria, "Concede +5 Destreza e -5 Sabedoria permanentemente."); }},
        {ItemID::TalismaCoruja, [criarTalisma]() { return criarTalisma(ItemID::TalismaCoruja, Propriedade::TalismaSabedoria, TipoAtributo::Sabedoria, TipoAtributo::Destreza, "Concede +5 Sabedoria e -5 Destreza permanentemente."); }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
