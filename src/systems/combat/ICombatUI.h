#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../rendering/raycaster/engine-raycaster/RaycasterHUD.h"

class Character;
class Item;

class ICombateUI {
public:
    virtual ~ICombateUI() = default;

    virtual void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) = 0;
    
    virtual void animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) = 0;
    virtual void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada = false) = 0;
    
    virtual void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) = 0;
    virtual void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) = 0;
    
    virtual void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) = 0;
    virtual void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) = 0;
    
    virtual void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) = 0;

    virtual void limparContextoPersonagemHUD() = 0;
    virtual void limparContextoInimigoMortoEDrops() = 0;

    virtual std::string margemCombate() = 0;

    virtual void adicionarMensagemFixa(const std::string& msg) = 0;
    virtual void limparMensagensFixas() = 0;
    virtual void definirMensagemBanner(const std::string& msg, CorBanner cor = CorBanner::OURO, const std::string& msgLinha2 = "") = 0;
    virtual void definirTurnoVisivel(int turno, const std::string& nome) = 0;
    
    virtual int obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) = 0;
    virtual int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) = 0;
    virtual int obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) = 0;
    virtual int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) = 0;
    
    virtual void notificarInimigosMaisAgeis() = 0;
    virtual void notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) = 0;
    virtual void notificarDesprevencaoInventario() = 0;
    virtual void notificarSemEscudos(const std::string& nomePersonagem) = 0;
    virtual void notificarDesequilibrioDefesa(const std::string& nomePersonagem) = 0;
    virtual void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) = 0;
    virtual void notificarAcaoInvalida() = 0;
    virtual void notificarCancelamentoItem() = 0;
    virtual void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) = 0;

    virtual void displayTelaVitoria(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas) = 0;
    virtual void displayTelaDerrota(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) = 0;
    
    virtual void displayTelaAtributos(Character* character) = 0;
    virtual void displayTelaDiario(Character* character) = 0;

    virtual void limparTela() = 0;
};
