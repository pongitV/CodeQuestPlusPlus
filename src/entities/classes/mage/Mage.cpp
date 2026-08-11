#include "Mage.h"

#include <iostream>
#include <memory>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../core/utils/Color.h"

// --- INFORMACOES DA CLASSE ---
std::string Mage::getNameClasse() const 
{
     return "Mage"; 
}

const std::vector<std::string>& Mage::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Mage::obterAtributosClasse() const
{
    return { 0, 5, 5, 3, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> Mage::obterEquipamentoClasse() const 
{
    auto equipamentos = ItemFactory::criarKitPocoes();

    equipamentos.push_back(ItemFactory::criarItem(ItemID::CajadoCristal));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::BarreiraMagica));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::Tunica));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Mage::getNamePassivaClasse() const 
{ 
    return "Foco arcano"; 
}

std::string Mage::obterDescricaoPassivaClasse() const 
{ 
    return "Ataques ressoam (25% em area) ou causam +25% de damage em alvo unico."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Mage::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Mage::getNameHabilidadeClasse() const 
{ 
    return "Canalizacao arcana"; 
}

std::string Mage::getClassAbilityDescription() const 
{ 
    return "Pula seu turno para se defender e dobra o damage no proximo turno. Recarga: 3 turnos."; 
}

void Mage::useClassAbility(Combat* /*combat*/, Character* personagemUsuario, std::vector<Character*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterRecargaHabilidade(AbilityID::CanalizacaoArcana);
    if (verificarEReportarRecarga(personagemUsuario, turnosRestantes, getNameHabilidadeClasse())) return;
    
    personagemUsuario->definirMultiplicador(2.0);
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2)); 
    personagemUsuario->definirCooldown(AbilityID::CanalizacaoArcana, 4);
    
    Item* escudo = personagemUsuario->obterEscudo();
    if (escudo) {
        personagemUsuario->definirDefendendo(true);
        std::string msg = DialogFunctions::formatarMsgHabilidade("Canalizacao arcana! Defendendo com " + escudo->getNameItem() + "! 2x Damage no prox. ataque!");
        notificarMensagemCombate(msg, msg);
    } else {
        std::string msg = DialogFunctions::formatarMsgHabilidade("Canalizacao arcana! Foco magico para 2x Damage no proximo ataque!");
        notificarMensagemCombate(msg, msg);
    }
}

// --- PROCESSAMENTO DE DANO  ---
int Mage::processarDanoPreAtaque(Character* /*atacante*/, Character* defensor, int baseDamage, bool isAtacanteJogador, size_t qtdInimigos) {
    if (defensor == nullptr) return baseDamage;
    if (!isAtacanteJogador || qtdInimigos <= 1) {
        int danoAumentado = static_cast<int>(baseDamage * 1.25);
        std::string logMsg = DialogFunctions::formatarMsgHabilidade("Foco Arcano: Damage concentrado aumentado em 25%!", Color::MAGENTA);
        notificarMensagemCombate(logMsg, logMsg);
        return danoAumentado;
    }
    return baseDamage;
}

void Mage::processarDanoPosAtaque(Character* atacante, Character* alvoAtual, Character* defensorPrincipal, int baseDamage, int danoPerfurante, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador, bool isArea, bool& ativouPassiva) {
    if (isAtacanteJogador && !isArea && alvoAtual != defensorPrincipal && alvoAtual->obterVida() > 0) {
        if (!ativouPassiva) {
            int danoAreaMsg = static_cast<int>(baseDamage * 0.25);
            std::string logMsg = DialogFunctions::formatarMsgHabilidade("Foco Arcano: A magia ressoa, causando " + std::to_string(danoAreaMsg) + " de damage aos enemies proximos!", Color::MAGENTA);
            notificarMensagemCombate(logMsg, logMsg);
            ativouPassiva = true;
        }
        int danoArea = static_cast<int>(baseDamage * 0.25);
        int perfuranteArea = static_cast<int>(danoPerfurante * 0.25);
        applyDamage(atacante, alvoAtual, danoArea, perfuranteArea);
    }
}
