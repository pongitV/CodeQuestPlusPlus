#include "CombatUIImpl.h"

#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../ui/screens/victory/ScreenVictory.h"
#include "../../ui/screens/defeat/ScreenDefeat.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/diary/ScreenDiary.h"
void CombateUIImpl::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    TelaCombate::configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo);
}

void CombateUIImpl::animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) {
    TelaCombate::animarIntroducaoCombate(titulo, enemies, currentPlayer);
}

void CombateUIImpl::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada) {
    TelaCombate::atualizarTelaEstatica(tituloCombate, listaDeInimigos, currentPlayer, listaDeAliados, animarEntrada);
}

void CombateUIImpl::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) {
    TelaCombate::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, currentPlayer, listaDeAliados, danoAnimacao);
}

void CombateUIImpl::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    TelaCombate::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
}

void CombateUIImpl::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) {
    TelaCombate::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, isParry, danoAnimacao);
}

void CombateUIImpl::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    TelaCombate::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
}

void CombateUIImpl::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) {
    TelaCombate::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, currentPlayer, listaDeAliados, drops);
}

void CombateUIImpl::limparContextoPersonagemHUD() {
    TelaCombate::contexto.personagemHUD = nullptr;
}

void CombateUIImpl::limparContextoInimigoMortoEDrops() {
    TelaCombate::contexto.inimigoMortoComDrops = nullptr;
    TelaCombate::contexto.dropsAtivos.clear();
}

std::string CombateUIImpl::margemCombate() {
    return TelaCombate::margemCombate();
}

void CombateUIImpl::adicionarMensagemFixa(const std::string& msg) {
    TelaCombate::adicionarMensagemFixa(msg);
}

void CombateUIImpl::limparMensagensFixas() {
    TelaCombate::limparMensagensFixas();
}

void CombateUIImpl::definirTurnoVisivel(int turno, const std::string& nome) {
    TelaCombate::definirTurnoVisivel(turno, nome);
}

int CombateUIImpl::obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return TelaCombate::obterAcaoDoJogador(turnoAtual, personagemAgindo, enemies, currentPlayer, aliados);
}

int CombateUIImpl::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return TelaCombate::obterAlvoAtaque(tituloCombate, enemies, currentPlayer, aliados);
}

int CombateUIImpl::obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return TelaCombate::obterAlvoItem(tituloCombate, enemies, currentPlayer, aliados);
}

int CombateUIImpl::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    return TelaCombate::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
}

void CombateUIImpl::notificarInimigosMaisAgeis() {
    TelaCombate::notificarInimigosMaisAgeis();
}

void CombateUIImpl::notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) {
    TelaCombate::notificarTurnoExtra(dexterityJogador, maxDestrezaInimigos);
}

void CombateUIImpl::notificarDesprevencaoInventario() {
    TelaCombate::notificarDesprevencaoInventario();
}

void CombateUIImpl::notificarSemEscudos(const std::string& nomePersonagem) {
    TelaCombate::notificarSemEscudos(nomePersonagem);
}

void CombateUIImpl::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    TelaCombate::notificarDesequilibrioDefesa(nomePersonagem);
}

void CombateUIImpl::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    TelaCombate::notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
}

void CombateUIImpl::notificarAcaoInvalida() {
    TelaCombate::notificarAcaoInvalida();
}

void CombateUIImpl::notificarCancelamentoItem() {
    TelaCombate::notificarCancelamentoItem();
}

void CombateUIImpl::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    TelaCombate::notificarRequisitoNaoAtendido(mensagemRequisito);
}

void CombateUIImpl::displayTelaVitoria(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas) {
    TelaVitoria::display(currentPlayer, amountDeOuroObtido, amountDeXpObtido, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns, itensObtidos, inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, novasDescobertas);
}

void CombateUIImpl::displayTelaDerrota(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) {
    TelaDerrota::display(currentPlayer, amountDeOuroObtido, amountDeXpObtido, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
}

void CombateUIImpl::displayTelaAtributos(Character* character) {
    TelaAtributos::gerenciarFichaDoJogador(character);
}

void CombateUIImpl::displayTelaDiario(Character* character) {
    TelaDiario::display(character);
}

void CombateUIImpl::limparTela() {
}
