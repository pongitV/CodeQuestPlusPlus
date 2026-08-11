#pragma once

#include <string>
#include <vector>
#include "CombatContext.h"
#include "../../../core/utils/Color.h"
class Character;
class Item;

class TelaCombate {
public:
    static CombatContexte contexto;

    static void displayLogoParaTelaDeCombate(const std::string& tituloDaTela = "", bool animar = true);
    static void animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr);
    static std::vector<std::string> obterLinhasBarraDeStatusDoJogador(Character* currentPlayer, Color corDestaque = Color::RESET, int danoAnimacao = -1, int frameAnimacao = 0, bool isCura = false);
    static void displayHordaDeInimigosLadoALado(const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao = nullptr, int frameAnimacao = 0, bool isCura = false, bool animarSurgimento = false, bool isMorte = false, Item* armaAtacante = nullptr, int danoAnimacao = -1, const std::vector<std::string>& dropsAnimacao = {});
    static void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao = -1);
    static void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao = 0);
    static void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados = {}, bool isParry = false, int danoAnimacao = -1);
    static void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados = {}, int curaAnimacao = 0);
    static void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops = {});
    static void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada = false);

    static void adicionarMensagemFixa(const std::string& msg);
    static void limparMensagensFixas();

    static void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo);
    static void definirTurnoVisivel(int turno, const std::string& nome);
    static void selecionarHUDDeAliado(Character* currentPlayer, const std::vector<Character*>& aliados);

    static int obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados);
    static int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados);
    static int obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados);
    static int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos);
    static void notificarInimigosMaisAgeis();
    static void notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos);
    static void notificarDesprevencaoInventario();
    static void notificarSemEscudos(const std::string& nomePersonagem);
    static void notificarDesequilibrioDefesa(const std::string& nomePersonagem);
    static void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo);
    static void notificarAcaoInvalida();
    static void notificarCancelamentoItem();
    static void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito);

    static std::string margemCombate();
};
