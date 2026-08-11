#include "Warrior.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../core/utils/Color.h"

// --- INFORMACOES DA CLASSE ---
std::string Warrior::getNameClasse() const 
{ 
    return "Knight"; 
}

const std::vector<std::string>& Warrior::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Warrior::obterAtributosClasse() const
{
    return { 0, 20, 10, 5, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Warrior::obterEquipamentoClasse() const 
{
    auto equipamentos = ItemFactory::criarKitPocoes();

    equipamentos.push_back(ItemFactory::criarItem(ItemID::EspadaFerro));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::EscudoMetal));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::ArmaduraMalha));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Warrior::getNamePassivaClasse() const 
{ 
    return "Golpe decisivo"; 
}

std::string Warrior::obterDescricaoPassivaClasse() const 
{ 
    return "Causa +10%/+20%/+30% de damage em enemies com menos de 30%/20%/10% de HP."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Warrior::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Warrior::getNameHabilidadeClasse() const 
{ 
    return "Grito de guerra"; 
}

std::string Warrior::getClassAbilityDescription() const 
{ 
    return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; 
}

void Warrior::useClassAbility(Combat* /*combat*/, Character* personagemUsuario, std::vector<Character*>& /*listaDeInimigos*/) 
{
    int turnosRestantes = personagemUsuario->obterRecargaHabilidade(AbilityID::Determinacao);
    if (verificarEReportarRecarga(personagemUsuario, turnosRestantes, getNameHabilidadeClasse())) return;

    if (personagemUsuario->possuiEfeito(EfeitoID::GritoDeGuerra)) {
        std::string msg = DialogFunctions::formatarMsgSistema("A habilidade " + getNameHabilidadeClasse() + " ja esta ativa!", Color::YELLOW);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int bonusForca = personagemUsuario->getStrength() / 2;
    int bonusDestreza = personagemUsuario->getDexterity() / 2;
    
    personagemUsuario->adicionarEfeito(std::make_unique<EfeitoGritoGuerra>(2, bonusForca, bonusDestreza));
    personagemUsuario->definirCooldown(AbilityID::Determinacao, 4);
    
    std::string msg = DialogFunctions::formatarMsgHabilidade("Grito de guerra! Forca +" + std::to_string(bonusForca) + " e Destreza +" + std::to_string(bonusDestreza) + "!");
    notificarMensagemCombate(msg, msg);
}

// --- PROCESSAMENTO DE DANO  ---
int Warrior::processarDanoPreAtaque(Character* /*atacante*/, Character* defensor, int baseDamage, bool /*isAtacanteJogador*/, size_t /*qtdInimigos*/) {
    int finalDamage = baseDamage;
    
    if (!defensor) return finalDamage;

    double percVida = (double)defensor->obterVida() / defensor->obterVidaMaxima();
    int bonus = 0;
    std::string state = "";
    
    if (percVida < 0.10) { bonus = 30; state = "nas ultimas"; }
    else if (percVida < 0.20) { bonus = 20; state = "gravemente ferido"; }
    else if (percVida < 0.30) { bonus = 10; state = "ferido"; }
    
    if (bonus > 0) {
        finalDamage = static_cast<int>(finalDamage * (1.0 + bonus / 100.0));
        std::string textoLog = DialogFunctions::formatarMsgHabilidade("Golpe Decisivo: O enemy esta " + state + "! Damage aumentado em " + std::to_string(bonus) + "%!", Color::RED);
        notificarMensagemCombate(textoLog, textoLog);
    }
    
    return finalDamage;
}
