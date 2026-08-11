#pragma once

#include "ICombatUI.h"

class CombateUIImpl : public ICombateUI {
public:
    CombateUIImpl() = default;
    ~CombateUIImpl() override = default;

    void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) override;
    
    void animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) override;
    void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada = false) override;
    
    void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) override;
    void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) override;
    
    void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) override;
    void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) override;
    
    void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) override;

    void limparContextoPersonagemHUD() override;
    void limparContextoInimigoMortoEDrops() override;

    std::string margemCombate() override;

    void adicionarMensagemFixa(const std::string& msg) override;
    void limparMensagensFixas() override;
    void definirMensagemBanner(const std::string& msg, CorBanner cor = CorBanner::OURO, const std::string& msgLinha2 = "") override {}
    void definirTurnoVisivel(int turno, const std::string& nome) override;
    
    int obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) override;
    int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) override;
    int obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) override;
    int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) override;
    
    void notificarInimigosMaisAgeis() override;
    void notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) override;
    void notificarDesprevencaoInventario() override;
    void notificarSemEscudos(const std::string& nomePersonagem) override;
    void notificarDesequilibrioDefesa(const std::string& nomePersonagem) override;
    void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) override;
    void notificarAcaoInvalida() override;
    void notificarCancelamentoItem() override;
    void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) override;

    void displayTelaVitoria(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas) override;
    void displayTelaDerrota(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) override;

    void displayTelaAtributos(Character* character) override;
    void displayTelaDiario(Character* character) override;

    void limparTela() override;
};
