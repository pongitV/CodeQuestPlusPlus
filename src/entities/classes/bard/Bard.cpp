#include "Bard.h"

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "../../../systems/combat/Combat.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/Constants.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../core/utils/InputControl.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../core/utils/Color.h"

// --- INFORMACOES DA CLASSE ---
std::string Bard::getNameClasse() const 
{
     return "Bard"; 
}

const std::vector<std::string>& Bard::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Bard::obterAtributosClasse() const
{
    return { 0, 10, 10, 3, 10, 10, 10};
}

std::vector<std::unique_ptr<Item>> Bard::obterEquipamentoClasse() const 
{
    auto equipamentos = ItemFactory::criarKitPocoes();
    
    equipamentos.push_back(ItemFactory::criarItem(ItemID::ViolaoEncantado));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::CapaMagica));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::TrajeNobre));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Bard::getNamePassivaClasse() const 
{ 
    return "Touch the sky"; 
}

std::string Bard::obterDescricaoPassivaClasse() const 
{ 
    return "Curas e buffs recebidos sao 40% mais fortes."; 
}

int Bard::processarCuraPassivaBard(int curaBase) const 
{
    return static_cast<int>(curaBase * Constants::BARD_HEAL_MULTIPLIER);
}

double Bard::processarMultiplicadorBuffPassivaBard(double multBase) const 
{
    if (multBase > 1.0) return 1.0 + (multBase - 1.0) * 1.4;
    return multBase;
}

// --- HABILIDADE DA CLASSE ---
std::string Bard::obterRecargaHabilidadeClasse() const 
{ 
    return "Recarga: 3 turnos (Individuais)."; 
}

std::string Bard::getNameHabilidadeClasse() const 
{ 
    return "Sinfonia do Bard"; 
}

std::string Bard::getClassAbilityDescription() const 
{ 
    return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; 
}

void Bard::useClassAbility(Combat* /*combat*/, Character* personagemUsuario, std::vector<Character*>& /*listaDeInimigos*/)
{
    struct SubHabilidade {
        AbilityID id;
        std::string nome;
        std::string descricao;
        std::function<void(Character*)> acao;
    };

    const std::array<SubHabilidade, 3> habilidades = {{
        { AbilityID::FlashingLights, "Flashing lights", "Cura e pula o turno", [this](Character* personagemHabilidade) {
            personagemHabilidade->definirPularTurnoInimigo(true);
            int cura = static_cast<int>((personagemHabilidade->getWisdom() * 2) + (personagemHabilidade->obterVidaMaxima() * 0.15));
            personagemHabilidade->modificarVida(cura);
            personagemHabilidade->definirCooldown(AbilityID::FlashingLights, 3);
            std::string msg = DialogFunctions::formatarMsgHabilidade("!Flashing lights! Voce recuperou " + std::to_string(cura) + " HP e encantou os enemies!", Color::GREEN);
            this->notificarMensagemCombate(msg, msg);
        }},
        { AbilityID::OnSight, "On sight", "1.5x Damage no proximo ataque", [this](Character* personagemHabilidade) {
            personagemHabilidade->definirMultiplicador(1.5);
            personagemHabilidade->definirCooldown(AbilityID::OnSight, 3);
            std::string msg = DialogFunctions::formatarMsgHabilidade(personagemHabilidade->getName() + " tocou 'On sight'! Proximo ataque com 1.5x damage!");
            this->notificarMensagemCombate(msg, msg);
        }},
        { AbilityID::ThroughTheWire, "Through the wire", "Metade do damage recebido", [this](Character* personagemHabilidade) {
            personagemHabilidade->adicionarEfeito(std::make_unique<EfeitoMetadeDano>(1));
            personagemHabilidade->definirCooldown(AbilityID::ThroughTheWire, 3);
            std::string msg = DialogFunctions::formatarMsgHabilidade("!Through the wire! Voce esta protegido contra metade do damage recebido!", Color::CYAN);
            this->notificarMensagemCombate(msg, msg);
        }}
    }};

    std::vector<std::string> opcoesHabilidades;
    for (size_t i = 0; i < habilidades.size(); ++i) {
        int cd = personagemUsuario->obterRecargaHabilidade(habilidades[i].id);
        opcoesHabilidades.push_back(habilidades[i].nome + " (" + habilidades[i].descricao + " | Recarga: " + std::to_string(cd) + ")");
    }
    opcoesHabilidades.push_back("CANCELAR");

    int escolha = InputControl::lerSelecaoMenuComSetas(opcoesHabilidades, false, TelaCombate::margemCombate());

    if (escolha == static_cast<int>(habilidades.size())) {
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }
    
    const auto& hab = habilidades[escolha];
    int cd = personagemUsuario->obterRecargaHabilidade(hab.id);
    if (verificarEReportarRecarga(personagemUsuario, cd, hab.nome)) return;

    hab.acao(personagemUsuario);
}
