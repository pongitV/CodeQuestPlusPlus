#include "Status.h"

#include <iostream>

#include "../../entities/classes/ClassBase.h"
#include "../../entities/character/Character.h"
#include "../utils/DialogFunctions.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../core/utils/Color.h"

void EfeitoSugaSangue::aplicarInicioTurno(Character* alvo) {
    if (!Character::isValido(atacante) || atacante->obterVida() <= 0) return;
    int danoRaizes = alvo->obterVida() / 5;
    if (danoRaizes > 0) 
    {
        alvo->modificarVida(-danoRaizes);
        atacante->modificarVida(danoRaizes);
        TelaCombate::adicionarMensagemFixa(alvo->getName() + " sofreu " + std::to_string(danoRaizes) + " de dano de Suga Sangue!");
    }
}

void EfeitoNecrose::aplicarInicioTurno(Character* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    TelaCombate::adicionarMensagemFixa(alvo->getName() + " sofreu " + std::to_string(danoPorTurno) + " de dano de Necrose!");
}

void EfeitoLentidao::aoEntrar(Character* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().dexterity = alvo->obterClasse()->aplicarPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().dexterity);
    else alvo->obterAtributosFinais().dexterity /= 2;
}

void EfeitoLentidao::aoSair(Character* alvo) {
    if (alvo->obterClasse()) alvo->obterAtributosFinais().dexterity = alvo->obterClasse()->reverterPenalidadeLentidaoPassivaArqueiro(alvo->obterAtributosFinais().dexterity);
    else alvo->obterAtributosFinais().dexterity *= 2;
}

void EfeitoFraqueza::aoEntrar(Character* alvo) {
    strengthPerdida = alvo->obterAtributosFinais().strength / 4;
    alvo->obterAtributosFinais().strength -= strengthPerdida;
}

void EfeitoFraqueza::aoSair(Character* alvo) {
    alvo->obterAtributosFinais().strength += strengthPerdida;
}

void EfeitoQuebraResistencia::aoEntrar(Character* alvo) {
    resistancePerdida = static_cast<int>(alvo->obterAtributosFinais().resistance * 0.20);
    constitutionPerdida = static_cast<int>(alvo->obterAtributosFinais().constitution * 0.10);
    alvo->obterAtributosFinais().resistance -= resistancePerdida;
    alvo->obterAtributosFinais().constitution -= constitutionPerdida;
}

void EfeitoQuebraResistencia::aoSair(Character* alvo) {
    alvo->obterAtributosFinais().resistance += resistancePerdida;
    alvo->obterAtributosFinais().constitution += constitutionPerdida;
}

void EfeitoQuebraResistencia::aplicarInicioTurno(Character* alvo) {
}

void EfeitoSangramento::aplicarInicioTurno(Character* alvo) {
    if (alvo->obterVida() <= 0) return;
    alvo->modificarVida(-danoPorTurno);
    TelaCombate::adicionarMensagemFixa(alvo->getName() + " sofreu " + std::to_string(danoPorTurno) + " de dano por Sangramento!");
}

int EfeitoMetadeDano::processarDanoRecebido(int damage) {
    int danoReduzido = damage / 2;
    return danoReduzido;
}

void EfeitoGritoGuerra::aoEntrar(Character* alvo) {
    alvo->obterAtributosFinais().strength += bonusForca;
    alvo->obterAtributosFinais().dexterity += bonusDestreza;
}

void EfeitoGritoGuerra::aoSair(Character* alvo) {
    alvo->obterAtributosFinais().strength -= bonusForca;
    alvo->obterAtributosFinais().dexterity -= bonusDestreza;
}

void EfeitoBuffAtributos::aoSair(Character* alvo) {
    if (alvo->obterMultiplicador() != 1.0) {
        alvo->definirMultiplicador(1.0);
    }
}

void EfeitoRodaAdaptacao::aplicarInicioTurno(Character* alvo) {
    if (alvo->obterVida() <= 0) return;
    if (!alvo->obterArmadura() || !alvo->obterArmadura()->temPropriedade(Propriedade::ArmaduraAdaptacao)) return;
    
    int cura = alvo->obterVidaMaxima() * 0.05;
    if (cura > 0) {
        std::string msg;
        msg.reserve(64);
        msg = TelaCombate::margemCombate();
        msg += ">> A Roda gira... Regenerou ";
        msg += std::to_string(cura);
        msg += " HP!\n";
        TelaCombate::adicionarMensagemFixa(msg);
    }
}

void EfeitoRodaAdaptacao::aoSair(Character* alvo) {
    alvo->obterAtributosFinais().strength -= bForca;
    alvo->obterAtributosFinais().dexterity -= bDestreza;
    alvo->obterAtributosFinais().resistance -= bResistencia;
    alvo->obterAtributosFinais().constitution -= bConstituicao;
    alvo->obterAtributosFinais().intelligence -= bInteligencia;
    alvo->obterAtributosFinais().wisdom -= bSabedoria;
    alvo->strengthrRecalculoCache();
}

void EfeitoRodaAdaptacao::adaptar(Character* alvo, Character* enemy) {
    if (!enemy) return;
    
    // --- 1. Adaptacao Defensiva (Baseada no enemy) ---
    int strengthFisicaInimigo = enemy->getStrength() + enemy->getDexterity();
    int strengthMagicaInimigo = enemy->getInteligencia() + enemy->getWisdom();
    
    std::string msgDefesa;
    if (strengthFisicaInimigo >= strengthMagicaInimigo) {
        alvo->alterarAtributoEstatico(TipoAtributo::Resistencia, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Constituicao, 2);
        bResistencia += 2; bConstituicao += 2;
        msgDefesa = "defesa fisica";
    } else {
        alvo->alterarAtributoEstatico(TipoAtributo::Sabedoria, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Constituicao, 2);
        bSabedoria += 2; bConstituicao += 2;
        msgDefesa = "defesa magica";
    }

    // --- 2. Adaptacao Ofensiva (Baseada na arma do jogador) ---
    int danoFisicoArma = 1;
    int danoMagicoArma = 0;
    if (alvo->obterArma()) {
        danoFisicoArma = alvo->obterArma()->obterDanoFisico();
        danoMagicoArma = alvo->obterArma()->obterDanoMagico();
    }

    std::string msgAtaque;
    if (danoMagicoArma > danoFisicoArma) {
        alvo->alterarAtributoEstatico(TipoAtributo::Inteligencia, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Sabedoria, 2);
        bInteligencia += 2; bSabedoria += 2;
        msgAtaque = "poder magico";
    } else if (danoFisicoArma > danoMagicoArma) {
        alvo->alterarAtributoEstatico(TipoAtributo::Forca, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Destreza, 2);
        bForca += 2; bDestreza += 2;
        msgAtaque = "poder fisico";
    } else {
        // Armas hibridas (ex: Espada de Exterminio)
        alvo->alterarAtributoEstatico(TipoAtributo::Forca, 2); 
        alvo->alterarAtributoEstatico(TipoAtributo::Destreza, 2);
        alvo->alterarAtributoEstatico(TipoAtributo::Inteligencia, 2);
        bForca += 2; bDestreza += 2; bInteligencia += 2;
        msgAtaque = "poder hibrido";
    }

    TelaCombate::adicionarMensagemFixa(TelaCombate::margemCombate() + "* KLINK! * A Roda adapta " + msgDefesa + " e " + msgAtaque + " (+2)!\n");
}
void EfeitoInviolavel::aoSair(Character* alvo) { alvo->adicionarEfeito(std::make_unique<EfeitoMiraCerteira>(99)); }
