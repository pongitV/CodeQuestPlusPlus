#include "ScreenAttributes.h"
#include "../../UIManager.h"
#include "../ScreenBase.h"
#include "../../../core/utils/InputControl.h"
#include "../../../core/utils/DialogFunctions.h"
#include "../../../systems/inventory/Item.h"

PoderCombate TelaAtributos::calcularPoderCombate(Character* currentPlayer, double multiplicador) {
    int danoFis = 1, danoMag = 0;
    if (currentPlayer->obterArma()) {
        danoFis = currentPlayer->obterArma()->obterDanoFisico();
        danoMag = currentPlayer->obterArma()->obterDanoMagico();
    }
    int strength = currentPlayer->getStrength();
    int dexterity = currentPlayer->getDexterity();
    int inteli = currentPlayer->getInteligencia();
    int wisdom = currentPlayer->getWisdom();

    if (danoFis == 0 && danoMag > 0) { strength /= 10; dexterity /= 10; }
    else if (danoFis > 0 && danoMag == 0) { inteli /= 10; wisdom /= 10; }

    PoderCombate p;
    p.danoFisEst = std::max(0, static_cast<int>((danoFis + strength) * (1.0 + (dexterity / 100.0)) * multiplicador));
    p.danoMagEst = std::max(0, static_cast<int>((danoMag + inteli) * (1.0 + (wisdom / 100.0)) * multiplicador));
    p.defFixa = currentPlayer->getResistance();
    p.mitigacao = std::min(50.0, currentPlayer->getConstitution() / 2.0);
    return p;
}

DebuffInfo TelaAtributos::calcularDebuff(Character* currentPlayer) {
    DebuffInfo d;
    d.temBuff = (currentPlayer->obterTurnosEfeito(EfeitoID::BuffAtributos) > 0 &&
                 currentPlayer->obterMultiplicador() > 1.0);
    d.strengthPerdida    = currentPlayer->possuiEfeito(EfeitoID::Fraqueza)     ? (currentPlayer->getStrength() / 3)      : 0;
    d.dexterityPerdida = currentPlayer->possuiEfeito(EfeitoID::Lentidao)     ? currentPlayer->getDexterity()       : 0;
    d.resPerdida      = currentPlayer->possuiEfeito(EfeitoID::QuebraResistencia) ? currentPlayer->getResistance() : 0;
    d.constPerdida    = currentPlayer->possuiEfeito(EfeitoID::QuebraResistencia) ? (currentPlayer->getConstitution() / 2) : 0;
    return d;
}

void TelaAtributos::display(Character* currentPlayer)
{
    GerenciadorPerspectiva::obterAtributosUI().display(currentPlayer);
}

void TelaAtributos::gerenciarFichaDoJogador(Character* currentPlayer)
{
    if (currentPlayer == nullptr) return;

    GerenciadorPerspectiva::obterAtributosUI().gerenciarFichaDoJogador(currentPlayer);
}
