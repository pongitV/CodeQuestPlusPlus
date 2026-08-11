#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../../entities/character/Character.h"
#include "../../systems/inventory/Item.h"
#include "../../core/utils/Color.h"

class ITelaCombateUI {
public:
    virtual ~ITelaCombateUI() = default;

    virtual void displayLogoParaTelaDeCombate(const std::string& tituloDaTela = "", bool animar = true) = 0;
    virtual void animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr) = 0;
    virtual std::vector<std::string> obterLinhasBarraDeStatusDoJogador(Character* currentPlayer, Color corDestaque = Color::RESET, int danoAnimacao = -1, int frameAnimacao = 0, bool isCura = false) = 0;
    virtual void displayHordaDeInimigosLadoALado(const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao = nullptr, int frameAnimacao = 0, bool isCura = false, bool animarSurgimento = false, bool isMorte = false, Item* armaAtacante = nullptr, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {}) = 0;
    virtual void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao = -1) = 0;
    virtual void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao = 0) = 0;
    virtual void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados = {}, bool isParry = false, int danoAnimacao = -1) = 0;
    virtual void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados = {}, int curaAnimacao = 0) = 0;
    virtual void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops = {}) = 0;
    virtual void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada = false, std::function<void()> callbackD2DOverlay = nullptr) = 0;
    virtual void adicionarMensagemFixa(const std::string& msg) = 0;
    virtual void limparMensagensFixas() = 0;
    virtual void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) = 0;
    virtual void definirTurnoVisivel(int turno, const std::string& nome) = 0;
    virtual void selecionarHUDDeAliado(Character* currentPlayer, const std::vector<Character*>& aliados) = 0;
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
};
