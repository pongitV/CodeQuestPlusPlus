#include "Necromancer.h"

#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "../../../ui/screens/combat/ScreenCombat.h"
#include "../../../systems/combat/Combat.h"
#include "../../../core/state/Status.h"
#include "../../../systems/inventory/ItemFactory.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../core/utils/InputControl.h"
#include "../../races/RaceBase.h"
#include "../../../core/utils/Color.h"

// --- INFORMACOES DA CLASSE ---
std::string Necromancer::getNameClasse() const {
    return "Necromancer";
}

const std::vector<std::string>& Necromancer::obterAparenciaClasseMenu() const {
    static const std::vector<std::string> appearance;
    return appearance;
}

Attributes Necromancer::obterAtributosClasse() const {
    return {-20, 5, 5, 3, 10, 10, 20};
}

std::vector<std::unique_ptr<Item>> Necromancer::obterEquipamentoClasse() const {
    auto equipamentos = ItemFactory::criarKitPocoes();
    equipamentos.push_back(ItemFactory::criarItem(ItemID::CajadoOsso));
    equipamentos.push_back(ItemFactory::criarItem(ItemID::RoupasRitualista));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Necromancer::getNamePassivaClasse() const {
    return "Toque Necrotico";
}

std::string Necromancer::obterDescricaoPassivaClasse() const {
    return "Ataques aplicam Necrose, causando 5% da Health Max. do alvo como damage por 3 turnos.\n"
           "Ao derrotar um enemy, coleta sua alma.";
}

void Necromancer::executarAtaqueComPassivaDaClasse(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& listaDeInimigos, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool aplicarPassiva) {
    // Comportamento padrão: apenas ataca o alvo principal ou todos se a arma for de área
    ClassBase::executarAtaqueComPassivaDaClasse(atacante, defensor, baseDamage, danoPerfurante, listaDeInimigos,
        [&](Character* atk, Character* def, int dmg, int perf) {
            // Callback para aplicar o damage e depois o efeito da passiva
            applyDamage(atk, def, dmg, perf);
            if (def->obterVida() > 0 && aplicarPassiva) {
                int danoNecrose = static_cast<int>(def->obterVidaMaxima() * 0.05);
                if (danoNecrose < 1) danoNecrose = 1;
                def->adicionarEfeito(std::make_unique<EfeitoNecrose>(3, danoNecrose));
                std::string msg = DialogFunctions::formatarMsgHabilidade("Necrose! " + def->getName() + " perdera " + std::to_string(danoNecrose) + " de HP por 3 turnos.", Color::MAGENTA);
                this->notificarMensagemCombate(msg, msg);
            }
        }, aplicarPassiva);
}


// --- HABILIDADE DA CLASSE ---
std::string Necromancer::obterRecargaHabilidadeClasse() const {
    return "Recarga: Nenhuma (consome 1 alma).";
}

std::string Necromancer::getNameHabilidadeClasse() const {
    return "Invocacao de Morto-Vivo";
}

std::string Necromancer::getClassAbilityDescription() const {
    return "Usa uma alma para invocar um clone com 80% dos attributes (Chefes 60%). Max: 3 lacaios.\nLacaios perdem 15% de sua Health Max a cada turno do jogador.";
}

void Necromancer::useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& /*listaDeInimigos*/) {
    bool temMiniBoss = false;
    int minionCount = 0;
    for (const auto& aliado : combat->obterAliadosVivosRaw()) {
        if (aliado->isMinion()) {
            minionCount++;
            if (aliado->isBoss()) {
                temMiniBoss = true;
            }
        }
    }

    if (temMiniBoss) {
        std::string msg = DialogFunctions::formatarMsgSistema("Seu Morto-Vivo Chefe exige todo o seu controle! Nao e possivel invocar mais lacaios.", Color::RED);
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    if (minionCount >= 3) {
        std::string msg = DialogFunctions::formatarMsgSistema("Limite maximo de 3 lacaios atingido!", Color::RED);
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    if (personagemUsuario->obterNumeroDeAlmas() == 0) {
        std::string msg = DialogFunctions::formatarMsgSistema("Voce nao possui almas para invocar!", Color::RED);
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int maxPossivel = std::min(3 - minionCount, static_cast<int>(personagemUsuario->obterNumeroDeAlmas()));

    std::vector<std::string> opcoesQtd;
    for (int i = 1; i <= maxPossivel; ++i) {
        if (i == 1) opcoesQtd.push_back("1 Morto-Vivo");
        else opcoesQtd.push_back(std::to_string(i) + " Mortos-Vivos (Enemy atua imediatamente)");
    }
    opcoesQtd.push_back("Cancelar");

    int qtdEscolhida = InputControl::lerSelecaoMenuComSetas(opcoesQtd, false, TelaCombate::margemCombate());
    if (qtdEscolhida == static_cast<int>(opcoesQtd.size()) - 1 || qtdEscolhida == -1) {
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int amountParaInvocar = qtdEscolhida + 1;
    std::vector<Character*> minionsRecemInvocados;

    for (int i = 0; i < amountParaInvocar; ++i) {
        std::vector<std::string> opcoes;
        auto& almas = personagemUsuario->obterAlmas();

        struct GrupoAlma {
            std::string nome;
            RaceType tipo;
            bool isBoss;
            int amount;
            int primeiroIndice;
        };
        std::vector<GrupoAlma> grupos;

        for (size_t j = 0; j < almas.size(); ++j) {
            bool encontrou = false;
            for (auto& g : grupos) {
                if (g.nome == almas[j]->getName()) {
                    g.amount++;
                    encontrou = true;
                    break;
                }
            }
            if (!encontrou) grupos.push_back({almas[j]->getName(), almas[j]->obterRaceType(), almas[j]->isBoss(), 1, static_cast<int>(j)});
        }

        for (const auto& g : grupos) {
            std::string prefixo = "";
            std::string cor = "";
            
            if (g.isBoss) {
                if (g.tipo == RaceType::Mahoraga) {
                    prefixo = "[CHEFE] ";
                    cor = "";
                } else {
                    prefixo = "[MINI-CHEFE] ";
                    cor = "";
                }
            }
            
            opcoes.push_back(cor + std::to_string(g.amount) + "x " + prefixo + "Morto-Vivo de " + g.nome + "");
        }
        opcoes.push_back("Cancelar Restante");

        int escolha = InputControl::lerSelecaoMenuComSetas(opcoes, false, TelaCombate::margemCombate());

        if (escolha == static_cast<int>(opcoes.size()) - 1 || escolha == -1) {
            if (i == 0) {
                personagemUsuario->definirHabilidadeCancelada(true);
                return;
            }
            break; // Para as invocacoes mas mantem as que ja foram feitas
        }

        int indiceRealParaRemover = grupos[escolha].primeiroIndice;
        auto minion = personagemUsuario->removerAlma(indiceRealParaRemover);
        std::string nomeOriginal = minion->getName();
        
        double fatorEscala = 0.8;
        if (minion->isBoss()) {
            fatorEscala = 0.6;
        }
        
        minion->escalarAtributos(fatorEscala);
        minion->setAsMinion(true);
        minion->alterarNome("Morto-Vivo (" + nomeOriginal + ")");

        std::string msg = DialogFunctions::formatarMsgHabilidade(personagemUsuario->getName() + " ergueu um Morto-Vivo de " + nomeOriginal + "!", Color::MAGENTA);
        notificarMensagemCombate(msg, msg);

        Character* minionPtr = minion.get();
        bool eraBoss = minionPtr->isBoss();
        combat->adicionarAliadoEmCombate(std::move(minion));
        minionsRecemInvocados.push_back(minionPtr);

        if (eraBoss) {
            if (i < amountParaInvocar - 1) {
                std::string msgBoss = DialogFunctions::formatarMsgSistema("A invocacao de um Chefe consumiu seu foco! Invocacoes adicionais canceladas.", Color::YELLOW);
                notificarMensagemCombate(msgBoss, msgBoss);
            }
            break; // Interrompe o laco, impedindo que os proximos mortos-vivos selecionados sejam invocados no mesmo turno
        }
    }
    
    // Se invocou mais de um minion na mesma acao, eles saltam seu turno ("Stun") para que o enemy atue de imediato!
    if (minionsRecemInvocados.size() > 1) {
        std::string msg = DialogFunctions::formatarMsgSistema("A invocacao multipla exauriu seu controle! O turno enemy comecara imediatamente!", Color::RED_CLARO);
        notificarMensagemCombate(msg, msg);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        for (auto* m : minionsRecemInvocados) {
            m->adicionarEfeito(std::make_unique<EfeitoAtordoamento>(1));
        }
    }
}
