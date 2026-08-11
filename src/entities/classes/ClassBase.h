#pragma once

#include "../character/Character.h"
#include "../../systems/inventory/Inventory.h"
#include "../../systems/inventory/Item.h"
#include "../../systems/inventory/ItemFactory.h"
#include <string>
#include <vector>
#include "../../core/utils/Color.h"
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include <memory>
#include <thread>
#include <chrono>
#include <functional>
#include <iostream>
#include <algorithm>

enum class ClassType 
{
    Nenhum,
    Archer,
    Bard,
    Warrior,
    Mage,
    NECROMANTE
};

enum class AbilityID
{
    Nenhuma,
    Determinacao,
    CanalizacaoArcana,
    FlashingLights,
    OnSight,
    ThroughTheWire,
    RetiradaComPontaria,
    InvocacaoDeEspectro
};

class Character;
class Item;
class Combat;

class ClassBase 
{
public:
    virtual ~ClassBase() = default;

    // INFORMACOES DA CLASSE
    virtual std::string getNameClasse() const = 0;
    virtual ClassType obterClassType() const = 0;
    virtual const std::vector<std::string>& obterAparenciaClasseMenu() const {
        static const std::vector<std::string> vazia;
        return vazia;
    }
    virtual std::string obterCaminhoSprite() const { return ""; }
    virtual Attributes obterAtributosClasse() const = 0;
    virtual std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const = 0;
 
    virtual std::string getNamePassivaClasse() const = 0;
    virtual std::string obterDescricaoPassivaClasse() const = 0;
    virtual std::string obterRecargaHabilidadeClasse() const = 0;

    // HABILIDADE DA CLASSE
    virtual std::string getNameHabilidadeClasse() const = 0;
    virtual std::string getClassAbilityDescription() const = 0;
    virtual void useClassAbility(Combat* combat, Character* personagemUsuario, std::vector<Character*>& listaDeInimigos) = 0;
    virtual TipoAtaque getAttackType() const { return TipoAtaque::UNICO; }
    virtual bool abilityConsumesTurn() const { return true; }

protected:
    void notificarMensagemCombate(const std::string& msgWithColor, const std::string& msgNoColor) const {
        std::string text = msgNoColor.empty() ? msgWithColor : msgNoColor;
        TelaCombate::adicionarMensagemFixa(text);
    }


    bool verificarEReportarRecarga(Character* personagemUsuario, int turnosRestantes, const std::string& nomeHabilidade) const {
        if (turnosRestantes > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            personagemUsuario->definirHabilidadeCancelada(true);
            return true;
        }
        return false;
    }

public:
    // PASSIVAS DE CLASSE
    virtual int processarCuraPassivaBard(int curaBase) const { return curaBase; }
    virtual double processarMultiplicadorBuffPassivaBard(double multBase) const { return multBase; }
    virtual int processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const { return penalidadeBase; }
    virtual int aplicarPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const { return dexterityAtual / 2; }
    virtual int reverterPenalidadeLentidaoPassivaArqueiro(int dexterityAtual) const { return dexterityAtual * 2; }

    // PROCESSAMENTO DE DANO
    virtual void executarAtaqueComPassivaDaClasse(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador) {

        baseDamage = processarDanoPreAtaque(atacante, defensor, baseDamage, isAtacanteJogador, enemies.size());

        bool isArea = atacante->getAttackType() == TipoAtaque::AREA && isAtacanteJogador && !enemies.empty();

        if (isArea) {
            executarAtaqueArea(atacante, defensor, baseDamage, danoPerfurante, enemies, applyDamage, isAtacanteJogador);
        } else {
            executarAtaqueUnico(atacante, defensor, baseDamage, danoPerfurante, enemies, applyDamage, isAtacanteJogador);
        }
    }

protected:
    virtual void executarAtaqueArea(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador) {
        std::string msgInfo = atacante->getName() + " desfere um ataque em area!";
        // A mensagem foi removida da UI para manter o combat limpo com Textos Flutuantes
        int danoDividido = std::max(1, baseDamage / static_cast<int>(enemies.size()));
        int perfuranteDividido = danoPerfurante / static_cast<int>(enemies.size());

        bool ativouPassiva = false;
        for (auto& inimigoAtual : enemies) {
            applyDamage(atacante, inimigoAtual.get(), danoDividido, perfuranteDividido);
            processarDanoPosAtaque(atacante, inimigoAtual.get(), defensor, baseDamage, danoPerfurante, applyDamage, isAtacanteJogador, true, ativouPassiva);
        }
    }

    virtual void executarAtaqueUnico(Character* atacante, Character* defensor, int baseDamage, int danoPerfurante, std::vector<std::unique_ptr<Character>>& enemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAtacanteJogador) {
        if (defensor != nullptr) {
            std::string msgInfo = atacante->getName() + " ataca " + defensor->getName() + "!";
            // A mensagem foi removida da UI para manter o combat limpo com Textos Flutuantes
            applyDamage(atacante, defensor, baseDamage, danoPerfurante);
        }

        bool ativouPassiva = false;
        for (auto& inimigoAtual : enemies) {
            processarDanoPosAtaque(atacante, inimigoAtual.get(), defensor, baseDamage, danoPerfurante, applyDamage, isAtacanteJogador, false, ativouPassiva);
        }
    }

    virtual int processarDanoPreAtaque(Character* /*atacante*/, Character* /*defensor*/, int baseDamage, bool /*isAtacanteJogador*/, size_t /*qtdInimigos*/) { return baseDamage; }
    virtual void processarDanoPosAtaque(Character* /*atacante*/, Character* /*alvoAtual*/, Character* /*defensorPrincipal*/, int /*baseDamage*/, int /*danoPerfurante*/, const std::function<void(Character*, Character*, int, int)>& /*applyDamage*/, bool /*isAtacanteJogador*/, bool /*isArea*/, bool& /*ativouPassiva*/) {}
};

template<typename Derived>
class ClassBaseCRTP : public ClassBase {
public:
    Derived& derived() { return static_cast<Derived&>(*this); }
    const Derived& derived() const { return static_cast<const Derived&>(*this); }
};
