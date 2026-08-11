#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <map>

#include "../common/EquipmentSlot.h"
#include "../interfaces/IAttacker.h"
#include "../interfaces/IDamageable.h"

#include "../../core/state/Status.h"
#include "../../systems/inventory/Inventory.h"
#include "../common/LevelSystem.h"
#include "../../core/utils/Color.h"

struct Attributes 
{
    int health;         // Maximum health points (HP) of the character
    int strength;        // Influences damage of frontal physical attacks and heavy weapons
    int dexterity;     // Determines turn order, damage of agile weapons and critical hit/dodge
    int resistance;  // Reduces physical damage received and acts as requirement for heavy shields
    int constitution; // General vitality, reduces debuff effectiveness and acts as requirement for armor
    int intelligence; // Base multiplier of magical damage and requirement for staffs/wands
    int wisdom;    // Increases secondary magical attributes, strength of heals and magical defense

    void addAttributes(const Attributes& outro) 
    {
        this->health += outro.health;
        this->strength += outro.strength;
        this->dexterity += outro.dexterity;
        this->resistance += outro.resistance;
        this->constitution += outro.constitution;
        this->intelligence += outro.intelligence;
        this->wisdom += outro.wisdom;
    }

    void adjustAll(int delta) {
        this->strength = std::max(1, this->strength + delta);
        this->dexterity = std::max(1, this->dexterity + delta);
        this->resistance = std::max(1, this->resistance + delta);
        this->constitution = std::max(1, this->constitution + delta);
        this->intelligence = std::max(1, this->intelligence + delta);
        this->wisdom = std::max(1, this->wisdom + delta);
    }

    void scaleAll(double factor) {
        this->health = std::max(1, static_cast<int>(this->health * factor));
        this->strength = static_cast<int>(this->strength * factor);
        this->dexterity = static_cast<int>(this->dexterity * factor);
        this->resistance = static_cast<int>(this->resistance * factor);
        this->constitution = static_cast<int>(this->constitution * factor);
        this->intelligence = static_cast<int>(this->intelligence * factor);
        this->wisdom = static_cast<int>(this->wisdom * factor);
    }
};

enum class TipoAtributo 
{
    Health = 1,
    Forca,
    Destreza,
    Resistencia,
    Constituicao,
    Inteligencia,
    Sabedoria
};

enum class TipoAtaque 
{
    UNICO,
    AREA
};

class RaceBase;   
class ClassBase; 
enum class ClassType;
enum class AbilityID;
enum class RaceType;

enum class DificuldadeJogo 
{
    Facil = 1,
    Normal = 2,
    Dificil = 3
};

/**
 * @brief PlayerClass central of the game that represents any living entity (Player, Enemies, NPCs).
 * Aggregates status, attributes, inventory and persistence and interaction logic.
 */
class Character : public IAttacker, public IDamageable
{
private:
    struct ControleCombate {
        bool estaDefendendo = false;
        std::vector<std::unique_ptr<Character>> almasColetadas;
        bool recargaDefesa = false;
        bool recargaHabilidade = false;
        bool pularTurnoInimigo = false;
        bool habilidadeCancelada = false;
        bool morteAnimada = false;
        double multiplicadorAtual = 1.0;
        int totalHealingReceived = 0;
        int vidaMaximaFixa = 0;
        std::unordered_map<AbilityID, int> cooldownsAtivos;
        
        void resetar() {
            estaDefendendo = false;
            recargaDefesa = false;
            recargaHabilidade = false;
            pularTurnoInimigo = false;
            habilidadeCancelada = false;
            morteAnimada = false;
            multiplicadorAtual = 1.0;
            totalHealingReceived = 0;
            vidaMaximaFixa = 0;
            cooldownsAtivos.clear();
        }
    };

    struct ControleSistema {
        bool querVoltarProMenu = false;
        bool labirintoDesbloqueado = false;
        bool podeReviver = true;
        bool parryAtivado = true;
        bool parryModerno = true;
        bool possuiRegeneracaoTroll = false;
        bool godModeAtivo = false;
        bool noclipAtivo = false;
        bool isMinion = false;
        DificuldadeJogo difficultyAtual = DificuldadeJogo::Normal;
        double difficultyMultiplicador = 1.0;
        char iconeJogador = '@';
        Color corJogador = Color::GREEN;
        Color corFundoTerminal = Color::RESET;
    };

    static std::unordered_set<Character*> personagensAtivos;

    ControleCombate combat;
    ControleSistema system;

protected:
    std::string nomePersonagem;
    int vidaAtual;
    std::unique_ptr<RaceBase> race;
    std::unique_ptr<ClassBase> classe;
    Attributes statsFinais;
    std::unique_ptr<Inventory> mochila;

    std::vector<std::unique_ptr<EfeitoStatus>> efeitosAtivos;
    std::vector<std::unique_ptr<EfeitoStatus>> efeitosFilaAdicao;
    std::vector<EfeitoID> efeitosFilaRemocao;
    bool processandoEfeitos = false;

    std::map<EquipmentSlot, Item*> equipamentos;
    Item* itemSelecionadoParaUso;

    // Cache de getters calculados
    // ATTENTION: This structure using 'mutable' is not thread-safe.
    // If the game starts using multi-threading (e.g., IA running in background), it is necessary to protect with std::mutex or std::atomic.
    struct CacheAtributos {
        int vidaMaxima = 0;
        int strength = 0;
        int dexterity = 0;
        int resistance = 0;
        int constitution = 0;
        int intelligence = 0;
        int wisdom = 0;
        int reducaoPercentual = 0;
        bool sujo = true;
    };
    mutable CacheAtributos cache_;
    mutable std::mutex mutexCache_; // Protects cache access in multithread environments

    struct CacheDano {
        std::pair<int, int> danoFisicoMagico = {0, 0};
        bool sujo = true;
        
        void invalidar() { sujo = true; }
    };
    mutable CacheDano cacheDano_;

    void atualizarCacheSeNecessario() const;
    
    
    std::unique_ptr<SistemaDeNivel> sistemaDeNivel;


    int* obterPonteiroAtributoEstatico(TipoAtributo atributo);

public:
    Character(const Character& other);
    Character(const std::string& nome, std::unique_ptr<RaceBase> r, std::unique_ptr<ClassBase> c);
    virtual ~Character();

    static bool isValido(Character* p);

    void calcularAtributos();
    std::unique_ptr<Character> clone() const;
    void mostrarStatus() const;
    void modificarVida(int valor);
    void alterarNome(const std::string& novoNome) { nomePersonagem = novoNome; }
    void equiparItem(Item* item);

    std::string getName() const { return nomePersonagem; }
    int obterVida() const { return vidaAtual; }
    int obterVidaMaxima() const {
        if (combat.vidaMaximaFixa > 0) return combat.vidaMaximaFixa;
        atualizarCacheSeNecessario(); return cache_.vidaMaxima;
    }
    int getStrength() const { atualizarCacheSeNecessario(); return cache_.strength; }
    int getDexterity() const { atualizarCacheSeNecessario(); return cache_.dexterity; }
    int getResistance() const { atualizarCacheSeNecessario(); return cache_.resistance; }
    int getConstitution() const { atualizarCacheSeNecessario(); return cache_.constitution; }
    int getInteligencia() const { atualizarCacheSeNecessario(); return cache_.intelligence; }
    int getWisdom() const { atualizarCacheSeNecessario(); return cache_.wisdom; }
    
    int getLevel() const { return sistemaDeNivel->getLevel(); }
    int getXpAtual() const { return sistemaDeNivel->getXpAtual(); }
    int getXpParaSubir() const { return sistemaDeNivel->getXpParaSubir(); }
    void definirNivel(int novoNivel) { sistemaDeNivel->definirNivel(novoNivel); }
    void definirXpAtual(int novoXp) { sistemaDeNivel->definirXpAtual(novoXp); }
    void definirXpParaSubir(int novoXpParaSubir) { sistemaDeNivel->definirXpParaSubir(novoXpParaSubir); }
    void definirVida(int novaVida) { vidaAtual = novaVida; }
    void ganharXp(int valor) { sistemaDeNivel->ganharXp(valor); }
    bool podeSubirDeNivel() const { return sistemaDeNivel->podeSubirDeNivel(); }

    // Professional English Property Accessors
    int getIntelligence() const { return getInteligencia(); }
    int getCurrentXp() const { return getXpAtual(); }
    int getRequiredXpForLevelUp() const { return getXpParaSubir(); }
    bool canLevelUp() const { return podeSubirDeNivel(); }
    bool subirDeNivel(TipoAtributo atributo);
    void escalarAtributos(double fator);
    void adicionarAlma(std::unique_ptr<Character> alma);
    std::vector<std::unique_ptr<Character>>& obterAlmas();
    size_t obterNumeroDeAlmas() const;
    std::unique_ptr<Character> removerAlma(int index);

    void strengthrRecalculoCache() { cache_.sujo = true; }
    
    int obterCuraTotalRecebida() const { return combat.totalHealingReceived; }

    void alterarAtributoEstatico(TipoAtributo atributo, int valor);
    Attributes& obterAtributosFinais() { return statsFinais; }

    RaceBase* obterRaca() const;
    ClassBase* obterClasse() const;
    std::string getNameClasse() const;
    ClassType obterClassType() const;
    RaceType obterRaceType() const;
    bool isBoss() const;
    
    Item* getEquippedAt(EquipmentSlot slot) const { auto it = equipamentos.find(slot); return it != equipamentos.end() ? it->second : nullptr; }
    Item* obterArma() const { return getEquippedAt(EquipmentSlot::MAO_PRINCIPAL); }
    Item* obterEscudo() const { return getEquippedAt(EquipmentSlot::MAO_SECUNDARIA); }
    Item* obterArmadura() const { return getEquippedAt(EquipmentSlot::ARMADURA); }
    Item* obterConsumivelRapido() const { return getEquippedAt(EquipmentSlot::CONSUMIVEL); }
    void desequiparConsumivel() { equipamentos.erase(EquipmentSlot::CONSUMIVEL); cache_.sujo = true; }
    Inventory* obterInventario() const { return mochila.get(); }
    Item* obterItemSelecionadoParaUso() const { return itemSelecionadoParaUso; }
    bool isItemEquipado(Item* item) const { 
        if (!item) return false;
        for (const auto& par : equipamentos) {
            if (par.second == item) return true;
        }
        return false;
    }

    bool estaEmCombate() const;
    void entrarEmCombate();
    void sairDoCombate();
    void prepararParaCombate();

    void definirItemSelecionadoParaUso(Item* item) { itemSelecionadoParaUso = item; }
    void ganharOuro(int valor) { mochila->adicionarOuro(valor); }
    void definirMultiplicador(double novoMultiplicador);
    double obterMultiplicador() const { return combat.multiplicadorAtual; }
    bool podeUsarRessurreicao() const { return system.podeReviver; }

    // English Methods & Aliases
    int getHealth() const { return obterVida(); }
    int getMaxHealth() const { return obterVidaMaxima(); }
    void setHealth(int h) { definirVida(h); }
    void setLevel(int l) { definirNivel(l); }
    void addGold(int g) { ganharOuro(g); }
    void addXp(int xp) { ganharXp(xp); }
    Inventory* getInventory() const { return obterInventario(); }
    RaceBase* getRace() const { return obterRaca(); }
    ClassBase* getClass() const { return obterClasse(); }
    Item* getWeapon() const { return obterArma(); }
    Item* getShield() const { return obterEscudo(); }
    Item* getArmor() const { return obterArmadura(); }
    bool isInCombat() const { return estaEmCombate(); }
    void enterCombat() { entrarEmCombate(); }
    void exitCombat() { sairDoCombate(); }
    void prepareForCombat() { prepararParaCombate(); }
    void consumirRessurreicao() { system.podeReviver = false; }

    int obterRecargaHabilidade(AbilityID habilidade) const 
    {
        auto it{combat.cooldownsAtivos.find(habilidade)};
        return (it != combat.cooldownsAtivos.end()) ? it->second : 0;
    }
    void definirCooldown(AbilityID habilidade, int turnos) 
    {
        combat.cooldownsAtivos[habilidade] = turnos;
    }
    
    bool obterHabilidadeCancelada() const { return combat.habilidadeCancelada; }
    void definirHabilidadeCancelada(bool foiCancelada) { combat.habilidadeCancelada = foiCancelada; }

    void definirRecarga(bool emRecarga) { combat.recargaHabilidade = emRecarga; }
    bool obterRecarga() const { return combat.recargaHabilidade; }
    void definirPularTurnoInimigo(bool pularTurno) { combat.pularTurnoInimigo = pularTurno; }
    bool obterPularTurnoInimigo() const { return combat.pularTurnoInimigo; }
    
    void definirVoltarProMenu(bool voltar) { system.querVoltarProMenu = voltar; }
    bool obterVoltarProMenu() const { return system.querVoltarProMenu; }
    void setReturnToMenu(bool returnToMenu) { definirVoltarProMenu(returnToMenu); }
    bool shouldReturnToMenu() const { return obterVoltarProMenu(); }

    void desbloquearLabirinto() { system.labirintoDesbloqueado = true; }
    bool obterLabirintoDesbloqueado() const { return system.labirintoDesbloqueado; }

    void desbloquearRegeneracaoTroll() { system.possuiRegeneracaoTroll = true; }
    bool possuiRegeneracaoTroll() const { return system.possuiRegeneracaoTroll; }

    void alternarGodMode() { system.godModeAtivo = !system.godModeAtivo; }
    bool isGodMode() const { return system.godModeAtivo; }

    void alternarNoclip() { system.noclipAtivo = !system.noclipAtivo; }
    bool isNoclip() const { return system.noclipAtivo; }

    void reduzirCooldowns();
    void prepararParaNovaBatalha();
    void finalizarBatalha();

    bool possuiEfeito(EfeitoID id) const;
    int obterTurnosEfeito(EfeitoID id) const;
    const EfeitoStatus* encontrarEfeito(EfeitoID id) const;


    void definirDefendendo(bool d) { combat.estaDefendendo = d; }
    bool obterDefendendo() const { return combat.estaDefendendo; }
    void definirRecargaDefesa(bool r) { combat.recargaDefesa = r; }
    bool obterRecargaDefesa() const { return combat.recargaDefesa; }
    void desequiparEscudo() { equipamentos.erase(EquipmentSlot::MAO_SECUNDARIA); cache_.sujo = true; }
    void desequiparArma() { equipamentos.erase(EquipmentSlot::MAO_PRINCIPAL); cache_.sujo = true; }
    void desequiparArmadura() { equipamentos.erase(EquipmentSlot::ARMADURA); cache_.sujo = true; }

    void definirMorteAnimada(bool m) { combat.morteAnimada = m; }
    bool obterMorteAnimada() const { return combat.morteAnimada; }

    void definirParryAtivado(bool p) { system.parryAtivado = p; }
    bool obterParryAtivado() const { return system.parryAtivado; }
    void definirParryModerno(bool m) { system.parryModerno = m; }
    bool obterParryModerno() const { return system.parryModerno; }

    void setAsMinion(bool minion) { system.isMinion = minion; }
    bool isMinion() const { return system.isMinion; }

    void definirDificuldade(DificuldadeJogo d) { system.difficultyAtual = d; }
    DificuldadeJogo obterDificuldade() const { return system.difficultyAtual; }
    void aplicarMultiplicadorDificuldade(double mult);

    void definirIconeJogador(char icone) { system.iconeJogador = icone; }
    char obterIconeJogador() const { return system.iconeJogador; }

    void definirCorJogador(Color cor) { system.corJogador = cor; }
    Color obterCorJogador() const { return system.corJogador; }

    void definirCorFundoTerminal(Color cor) { system.corFundoTerminal = cor; }
    Color obterCorFundoTerminal() const { return system.corFundoTerminal; }

    TipoAtaque getAttackType() const;
    bool habilidadeDaClasseConsomeTurno() const;

    void adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito);
    void processarEfeitosInicioTurno();
    bool podeAgir(std::string& outMotivoIncapacidade) const;

    // Fills the vector with the IDs of all active effects (avoids unwanted allocations)
    void obterIDsEfeitosAtivos(std::vector<EfeitoID>& outIDs) const;
    void limparEfeitos();
    void removerEfeito(EfeitoID id);

    int calcularDefesaBase(int danoBruto, int danoPerfurante) override;
    ResultadoDano receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas = true) override;
    std::pair<int, int> calcularDanoOfensivoBase() override;
    int garantirDanoMinimo(int danoAtual) override;

    virtual void executarDrops(Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal);
};
