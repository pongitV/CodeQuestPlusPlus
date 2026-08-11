#include "ScreenCombat.h"
#include "../../UIManager.h"

#include "../../../entities/character/Character.h"
#include "../../../core/utils/Color.h"
CombatContexte TelaCombate::contexto;

void TelaCombate::displayLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) {
    GerenciadorPerspectiva::obterTelaCombateUI().displayLogoParaTelaDeCombate(tituloDaTela, animar);
}

void TelaCombate::animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarIntroducaoCombate(titulo, enemies, currentPlayer);
}

std::vector<std::string> TelaCombate::obterLinhasBarraDeStatusDoJogador(Character* currentPlayer, Color corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterLinhasBarraDeStatusDoJogador(currentPlayer, corDestaque, danoAnimacao, frameAnimacao, isCura);
}

void TelaCombate::displayHordaDeInimigosLadoALado(const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().displayHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frameAnimacao, isCura, animarSurgimento, isMorte, armaAtacante, danoAnimacao, dropsAnimacao);
}

void TelaCombate::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, currentPlayer, listaDeAliados, danoAnimacao);
}

void TelaCombate::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
}

void TelaCombate::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, isParry, danoAnimacao);
}

void TelaCombate::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
}

void TelaCombate::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) {
    GerenciadorPerspectiva::obterTelaCombateUI().animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, currentPlayer, listaDeAliados, drops);
}

void TelaCombate::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada) {
    GerenciadorPerspectiva::obterTelaCombateUI().atualizarTelaEstatica(tituloCombate, listaDeInimigos, currentPlayer, listaDeAliados, animarEntrada);
}

void TelaCombate::adicionarMensagemFixa(const std::string& msg) {
    GerenciadorPerspectiva::obterTelaCombateUI().adicionarMensagemFixa(msg);
}

void TelaCombate::limparMensagensFixas() {
    GerenciadorPerspectiva::obterTelaCombateUI().limparMensagensFixas();
}

void TelaCombate::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    contexto.isModo3D = modo3D;
    contexto.matrizDoMapaAtual = matriz;
    contexto.jogadorPosX = posX;
    contexto.jogadorPosY = posY;
    contexto.jogadorAngulo = angulo;
    contexto.tituloMapaAtual = titulo;
}

void TelaCombate::selecionarHUDDeAliado(Character* currentPlayer, const std::vector<Character*>& aliados) {
    GerenciadorPerspectiva::obterTelaCombateUI().selecionarHUDDeAliado(currentPlayer, aliados);
}

void TelaCombate::definirTurnoVisivel(int turno, const std::string& nome) {
    contexto.turnoAtualVisivel = turno;
    contexto.nomeTurnoVisivel = nome;
}

int TelaCombate::obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterAcaoDoJogador(turnoAtual, personagemAgindo, enemies, currentPlayer, aliados);
}

int TelaCombate::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterAlvoAtaque(tituloCombate, enemies, currentPlayer, aliados);
}

int TelaCombate::obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterAlvoItem(tituloCombate, enemies, currentPlayer, aliados);
}

int TelaCombate::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    return GerenciadorPerspectiva::obterTelaCombateUI().obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
}

void TelaCombate::notificarInimigosMaisAgeis() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarInimigosMaisAgeis();
}

void TelaCombate::notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarTurnoExtra(dexterityJogador, maxDestrezaInimigos);
}

void TelaCombate::notificarDesprevencaoInventario() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarDesprevencaoInventario();
}

void TelaCombate::notificarSemEscudos(const std::string& nomePersonagem) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarSemEscudos(nomePersonagem);
}

void TelaCombate::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarDesequilibrioDefesa(nomePersonagem);
}

void TelaCombate::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
}

void TelaCombate::notificarAcaoInvalida() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarAcaoInvalida();
}

void TelaCombate::notificarCancelamentoItem() {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarCancelamentoItem();
}

void TelaCombate::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    GerenciadorPerspectiva::obterTelaCombateUI().notificarRequisitoNaoAtendido(mensagemRequisito);
}

std::string TelaCombate::margemCombate() {
    int larguraHUD = 91;
    int larguraTerminal = 120;
    int larguraRef = std::min(larguraHUD, larguraTerminal);
    return "";
}
