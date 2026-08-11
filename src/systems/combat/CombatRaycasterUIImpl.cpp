#include "CombatRaycasterUIImpl.h"

#include "../../rendering/raycaster/screens/combat/ScreenCombatRaycaster.h"
#include "../../rendering/raycaster/screens/victory/ScreenVictoryRaycaster.h"
#include "../../rendering/raycaster/screens/defeat/ScreenDefeatRaycaster.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/diary/ScreenDiary.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "Parry.h"

void CombateRaycasterUIImpl::configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    TelaCombateRaycaster::configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo);
}

void CombateRaycasterUIImpl::animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) {
    TelaCombateRaycaster::animarIntroducaoCombate(titulo, enemies, currentPlayer);
}

void CombateRaycasterUIImpl::atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada) {
    Parry::onUpdateScreen = [=]() {
        TelaCombateRaycaster::atualizarTelaEstatica(tituloCombate, listaDeInimigos, currentPlayer, listaDeAliados, false, nullptr);
    };
    TelaCombateRaycaster::atualizarTelaEstatica(tituloCombate, listaDeInimigos, currentPlayer, listaDeAliados, animarEntrada, nullptr);
}

void CombateRaycasterUIImpl::animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) {
    TelaCombateRaycaster::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, currentPlayer, listaDeAliados, danoAnimacao);
}

void CombateRaycasterUIImpl::animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    TelaCombateRaycaster::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
}

void CombateRaycasterUIImpl::animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) {
    TelaCombateRaycaster::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, isParry, danoAnimacao);
}

void CombateRaycasterUIImpl::animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) {
    TelaCombateRaycaster::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao);
}

void CombateRaycasterUIImpl::animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) {
    TelaCombateRaycaster::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, currentPlayer, listaDeAliados, drops);
}

void CombateRaycasterUIImpl::limparContextoPersonagemHUD() {
    TelaCombate::contexto.personagemHUD = nullptr;
}

void CombateRaycasterUIImpl::limparContextoInimigoMortoEDrops() {
    TelaCombate::contexto.inimigoMortoComDrops = nullptr;
    TelaCombate::contexto.dropsAtivos.clear();
}

std::string CombateRaycasterUIImpl::margemCombate() {
    return ""; // TelaCombateRaycaster usually uses its own formatting or empty margin for fixed messages in 3D
}

void CombateRaycasterUIImpl::adicionarMensagemFixa(const std::string& msg) {
    TelaCombateRaycaster::adicionarMensagemFixa(msg);
}

void CombateRaycasterUIImpl::limparMensagensFixas() {
    TelaCombateRaycaster::limparMensagensFixas();
}

void CombateRaycasterUIImpl::definirMensagemBanner(const std::string& msg, CorBanner cor, const std::string& msgLinha2) {
    TelaCombateRaycaster::definirMensagemBanner(msg, cor, msgLinha2);
}

void CombateRaycasterUIImpl::definirTurnoVisivel(int turno, const std::string& nome) {
    TelaCombateRaycaster::definirTurnoVisivel(turno, nome);
}

int CombateRaycasterUIImpl::obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return TelaCombateRaycaster::obterAcaoDoJogador(turnoAtual, personagemAgindo, enemies, currentPlayer, aliados);
}

int CombateRaycasterUIImpl::obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return TelaCombateRaycaster::obterAlvoAtaque(tituloCombate, enemies, currentPlayer, aliados);
}

int CombateRaycasterUIImpl::obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) {
    return TelaCombateRaycaster::obterAlvoItem(tituloCombate, enemies, currentPlayer, aliados);
}

int CombateRaycasterUIImpl::obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) {
    return TelaCombateRaycaster::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos);
}

void CombateRaycasterUIImpl::notificarInimigosMaisAgeis() {
    TelaCombateRaycaster::notificarInimigosMaisAgeis();
}

void CombateRaycasterUIImpl::notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) {
    TelaCombateRaycaster::notificarTurnoExtra(dexterityJogador, maxDestrezaInimigos);
}

void CombateRaycasterUIImpl::notificarDesprevencaoInventario() {
    TelaCombateRaycaster::notificarDesprevencaoInventario();
}

void CombateRaycasterUIImpl::notificarSemEscudos(const std::string& nomePersonagem) {
    TelaCombateRaycaster::notificarSemEscudos(nomePersonagem);
}

void CombateRaycasterUIImpl::notificarDesequilibrioDefesa(const std::string& nomePersonagem) {
    TelaCombateRaycaster::notificarDesequilibrioDefesa(nomePersonagem);
}

void CombateRaycasterUIImpl::notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) {
    TelaCombateRaycaster::notificarPosturaDefensiva(nomePersonagem, nomeEscudo);
}

void CombateRaycasterUIImpl::notificarAcaoInvalida() {
    TelaCombateRaycaster::notificarAcaoInvalida();
}

void CombateRaycasterUIImpl::notificarCancelamentoItem() {
    TelaCombateRaycaster::notificarCancelamentoItem();
}

void CombateRaycasterUIImpl::notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) {
    TelaCombateRaycaster::notificarRequisitoNaoAtendido(mensagemRequisito);
}

void CombateRaycasterUIImpl::displayTelaVitoria(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas) {
    std::unordered_map<std::string, int> frequenciaDrops;
    for (const auto& item : itensObtidos) {
        frequenciaDrops[item]++;
    }
    std::vector<std::pair<std::string, int>> dropsUnicos;
    for (const auto& par : frequenciaDrops) {
        dropsUnicos.push_back(par);
    }

    bool podeSubirNivel = currentPlayer->getXpAtual() + amountDeXpObtido >= currentPlayer->getXpParaSubir();

    TelaVitoriaRaycaster::display(currentPlayer, amountDeOuroObtido, amountDeXpObtido, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns, inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, dropsUnicos, podeSubirNivel, novasDescobertas, "");
}

void CombateRaycasterUIImpl::displayTelaDerrota(Character* currentPlayer, int amountDeOuroObtido, int amountDeXpObtido, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) {
    TelaDerrotaRaycaster::display(currentPlayer, amountDeOuroObtido, amountDeXpObtido, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
}

void CombateRaycasterUIImpl::displayTelaAtributos(Character* character) {
    TelaAtributos::gerenciarFichaDoJogador(character);
}

void CombateRaycasterUIImpl::displayTelaDiario(Character* character) {
    TelaDiario::display(character);
}

void CombateRaycasterUIImpl::limparTela() {
}
