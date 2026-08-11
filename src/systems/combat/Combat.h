#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "ICombatUI.h"
#include "../../entities/character/Character.h"

/**
 * @brief PlayerClass responsavel por gerenciar o fluxo de combat do game.
 * Controla turnos, health, acoes e UI do combat.
 */
class Combat 
{
public:
    enum class AcaoCombate 
    { 
        Atacar = 1, 
        Defender = 2, 
        Habilidade = 3, 
        Inventory = 4, 
        Jogador = 5, 
        Bestiary = 6 
    };

private:
    // Referencias aos participantes do combat ativo
    // currentPlayer: Pointer para o personagem controlado pelo jogador
    //              Mantém referência para o owner do combat
    //              Nunca nullptr durante execução
    Character* currentPlayer;
    std::vector<std::unique_ptr<Character>> listaDeInimigos;
    std::vector<std::unique_ptr<Character>> listaDeAliados;

    // Interface visual de combat (Injecao de Dependencia)
    std::unique_ptr<ICombateUI> ui;

    // Constantes de Controle
    static constexpr int DANO_MAXIMO_DEBUG = 99999;
    static constexpr int DANO_NULO = 0;

    // Estatisticas gerais e controle da sessao de combat
    int ouroObtido;
    int xpObtido;
    int totalDamageDealt;
    int totalDamageTaken;
    int contadorDoTurnoAtual;
    std::vector<std::string> itensObtidos;
    std::vector<std::string> inimigosDerrotados;

    // Estatisticas Avancadas da Sessao
    int stats_parriesTentados;
    int stats_parriesEfetivos;
    int stats_parriesPerfeitos;
    int stats_maiorDanoCausado;
    int stats_itensConsumidos;
    std::vector<std::string> stats_novasDescobertas;
    void resetarEstatisticasAvancadas();

    std::string getNameFormatadoComNumero(Character* c) const;
    void applyDamageAoAlvo(Character* personagemAtacante, Character* personagemAlvo, int danoBruto, int danoPerfurante, int turnoAtualDoCombate);
    void processarMorteDeInimigo(Character* enemy);
    void displayResultadoDoAtaque(Character* atacante, Character* alvo, int finalDamage, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado);

    void prepararTurnoPersonagem(Character* character);
    void processarPosDano(Character* atacante, Character* alvo, int finalDamage, bool tentouParry, bool parrySucesso);
    bool ehPersonagemJogadorOuAliado(Character* character) const;
    void processarMenuDeAcoesDoJogador(Character* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void processarAcaoAtacar(Character* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoDefender(Character* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoHabilidade(Character* personagemAgindo, bool& turnoFoiConsumido);
    void processarAcaoInventario(Character* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno);
    void limparInimigosMortos();
    Item* selecionarEscudo(Character* personagemAgindo);

    std::string obterTituloDoCombate() const;
    std::vector<Character*> obterInimigosRaw() const;
    void displayTelaDeCombate(bool animarEntrada = false) const;

public:
    /**
     * @brief Construtor que move ownership dos inimigos
     * @param jogador Pointer para o jogador (não move ownership)
     * @param inimigos Inimigos com ownership transferido
     */
    Combat(Character* jogadorParaOCombate,
           std::vector<std::unique_ptr<Character>>&& inimigosParaOCombate,
           std::unique_ptr<ICombateUI> interfaceVisual = nullptr);
    virtual ~Combat();

    void setContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo);

    std::vector<Character*> obterAliadosVivosRaw() const;
    bool executarTurnoJogadorOuAliado(Character* character, bool& primeiraRenderizacao, bool processarEfeitosInicio = true);
    void adicionarAliadoEmCombate(std::unique_ptr<Character> aliado);
    void adicionarAliados(std::vector<std::unique_ptr<Character>> aliados);
    /**
     * @brief Inicia o laco principal de combat.
     */
    void iniciarCombate();

    /**
     * @brief Executa a intelligence e as acoes de todos os enemies presentes.
     */
    void executarTurnoDeTodosOsInimigos();

    /**
     * @brief Verifica se todos os enemies estao mortos ou se o jogador morreu.
     * @return true se o combat deve acabar.
     */
    bool verificarCondicaoDeVitoriaOuDerrota();

    /**
     * @brief Aplica o fluxo completo de damage fisico de um character a outro.
     * @param personagemAtacante Ponteiro para quem ataca.
     * @param personagemDefensor Ponteiro para quem defende.
     * @param turnoAtualDoCombate Turno em que a acao ocorre.
     */
    void realizarAtaqueFisico(Character* personagemAtacante, Character* personagemDefensor, int turnoAtualDoCombate);

    int obterParriesTentados() const { return stats_parriesTentados; }
    int obterParriesEfetivos() const { return stats_parriesEfetivos; }
    int obterMaiorDanoCausado() const { return stats_maiorDanoCausado; }
    int obterItensConsumidos() const { return stats_itensConsumidos; }
    const std::vector<std::string>& obterNovasDescobertas() const { return stats_novasDescobertas; }

    // English Aliases
    void startCombat() { iniciarCombate(); }
    void executeAllEnemiesTurn() { executarTurnoDeTodosOsInimigos(); }
    bool checkWinLossCondition() { return verificarCondicaoDeVitoriaOuDerrota(); }
    void performPhysicalAttack(Character* attacker, Character* defender, int turn) { realizarAtaqueFisico(attacker, defender, turn); }
    void set3DContext(bool m3d, const std::vector<std::string>& mat, float px, float py, float ang, const std::string& title) { setContexto3D(m3d, mat, px, py, ang, title); }
    void addAllyInCombat(std::unique_ptr<Character> ally) { adicionarAliadoEmCombate(std::move(ally)); }
};

struct CombatUIBatch {
    std::vector<std::string> mensagens;
    std::vector<std::pair<Character*, int>> danosAnimados;
    
    void limpar() {
        mensagens.clear();
        danosAnimados.clear();
    }
};

struct CombatState {
    int turnoAtual = 0;
    std::vector<Character*> ordemDeAcao;
    bool combateMudou = true;
};

struct CombatPrediction {
    std::vector<std::pair<Character*, int>> predictedAttacks;
    std::chrono::steady_clock::time_point predictionTimestamp;
    static constexpr auto PREDICTION_DURATION = std::chrono::milliseconds(500);

    bool precisaRecalcular() const {
        return (std::chrono::steady_clock::now() - predictionTimestamp) > PREDICTION_DURATION;
    }
};

struct DamageCache {
    std::unordered_map<Character*, int> baseDamage;
    std::unordered_map<Character*, int> defenseReduction;
    bool sujo = true;

    void invalidar() { sujo = true; }
};

using CombatAction = Combat::AcaoCombate;
