#include "UIManager.h"
#include "../rendering/raycaster/engine-raycaster/Raycaster.h"
#include "../rendering/raycaster/engine-raycaster/RaycasterRendererImpl.h"
#include "../core/utils/RendererProvider.h"
#include "../rendering/raycaster/ScreenManager.h"
#include "../rendering/raycaster/screens/diary/ScreenDiaryRaycaster.h"
#include "../rendering/raycaster/screens/inventory/ScreenInventoryRaycaster.h"
#include "../rendering/raycaster/screens/attributes/ScreenAttributesRaycaster.h"
#include "../rendering/raycaster/screens/bestiary/ScreenBestiaryRaycaster.h"
#include "../rendering/raycaster/screens/combat/ScreenCombatRaycaster.h"
#include "../rendering/raycaster/screens/defeat/ScreenDefeatRaycaster.h"
#include "../rendering/raycaster/screens/victory/ScreenVictoryRaycaster.h"
#include "../rendering/raycaster/screens/pause/ScreenPauseRaycaster.h"
#include "../rendering/raycaster/screens/map/ScreenMapWorldRaycaster.h"
#include "../core/utils/Color.h"

// Adaptadores de interface
// Envoltorios leves que implementam as interfaces abstratas de UI delegando
// para os metodos estaticos dos renderizadores do Raycaster.
// Mantem as dependencias concretas isoladas na raiz de composicao.

class AtributosUIAdapter : public IAtributosUI {
    void display(Character* jogador) override { TelaAtributosRaycaster::display(jogador); }
    void displayDetalhesAtributos(Character* currentPlayer) override { TelaAtributosRaycaster::displayDetalhesAtributos(currentPlayer); }
    void gerenciarFichaDoJogador(Character* currentPlayer) override { TelaAtributosRaycaster::gerenciarFichaDoJogador(currentPlayer); }
};

class BestiarioUIAdapter : public IBestiarioUI {
    void display(const std::vector<Character*>& enemies) override { TelaBestiarioRaycaster::display(enemies); }
    void displayDetalhe(Character* enemy) override { TelaBestiarioRaycaster::displayDetalhe(enemy); }
};

class TelaCombateUIAdapter : public ITelaCombateUI {
    void displayLogoParaTelaDeCombate(const std::string& tituloDaTela, bool animar) override { TelaCombateRaycaster::displayLogoParaTelaDeCombate(tituloDaTela, animar); }
    void animarIntroducaoCombate(const std::string& titulo, const std::vector<Character*>& enemies, Character* currentPlayer) override { TelaCombateRaycaster::animarIntroducaoCombate(titulo, enemies, currentPlayer); }
    std::vector<std::string> obterLinhasBarraDeStatusDoJogador(Character* currentPlayer, Color corDestaque, int danoAnimacao, int frameAnimacao, bool isCura) override { return TelaCombateRaycaster::obterLinhasBarraDeStatusDoJogador(currentPlayer, corDestaque, danoAnimacao, frameAnimacao, isCura); }
    void displayHordaDeInimigosLadoALado(const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, int frameAnimacao, bool isCura, bool animarSurgimento, bool isMorte, Item* armaAtacante, int danoAnimacao, const std::vector<std::string>& dropsAnimacao) override { TelaCombateRaycaster::displayHordaDeInimigosLadoALado(listaDeInimigos, alvoAnimacao, frameAnimacao, isCura, animarSurgimento, isMorte, armaAtacante, danoAnimacao, dropsAnimacao); }
    void animarDanoNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* atacante, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int danoAnimacao) override { TelaCombateRaycaster::animarDanoNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, atacante, currentPlayer, listaDeAliados, danoAnimacao); }
    void animarCuraNoInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) override { TelaCombateRaycaster::animarCuraNoInimigo(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao); }
    void animarDanoNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool isParry, int danoAnimacao) override { TelaCombateRaycaster::animarDanoNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, isParry, danoAnimacao); }
    void animarCuraNoJogador(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* alvoAnimacao, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, int curaAnimacao) override { TelaCombateRaycaster::animarCuraNoJogador(tituloCombate, listaDeInimigos, alvoAnimacao, currentPlayer, listaDeAliados, curaAnimacao); }
    void animarMorteInimigo(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* inimigoMorto, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, const std::vector<std::string>& drops) override { TelaCombateRaycaster::animarMorteInimigo(tituloCombate, listaDeInimigos, inimigoMorto, currentPlayer, listaDeAliados, drops); }
    void atualizarTelaEstatica(const std::string& tituloCombate, const std::vector<Character*>& listaDeInimigos, Character* currentPlayer, const std::vector<Character*>& listaDeAliados, bool animarEntrada, std::function<void()> callbackD2DOverlay) override { TelaCombateRaycaster::atualizarTelaEstatica(tituloCombate, listaDeInimigos, currentPlayer, listaDeAliados, animarEntrada, callbackD2DOverlay); }
    void adicionarMensagemFixa(const std::string& msg) override { TelaCombateRaycaster::adicionarMensagemFixa(msg); }
    void limparMensagensFixas() override { TelaCombateRaycaster::limparMensagensFixas(); }
    void configurarContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) override { TelaCombateRaycaster::configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo); }
    void definirTurnoVisivel(int turno, const std::string& nome) override { TelaCombateRaycaster::definirTurnoVisivel(turno, nome); }
    void selecionarHUDDeAliado(Character* currentPlayer, const std::vector<Character*>& aliados) override { TelaCombateRaycaster::selecionarHUDDeAliado(currentPlayer, aliados); }
    int obterAcaoDoJogador(int turnoAtual, Character* personagemAgindo, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) override { return TelaCombateRaycaster::obterAcaoDoJogador(turnoAtual, personagemAgindo, enemies, currentPlayer, aliados); }
    int obterAlvoAtaque(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) override { return TelaCombateRaycaster::obterAlvoAtaque(tituloCombate, enemies, currentPlayer, aliados); }
    int obterAlvoItem(const std::string& tituloCombate, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& aliados) override { return TelaCombateRaycaster::obterAlvoItem(tituloCombate, enemies, currentPlayer, aliados); }
    int obterEscolhaDeEscudo(const std::string& nomePersonagem, const std::vector<Item*>& listaDeEscudos) override { return TelaCombateRaycaster::obterEscolhaDeEscudo(nomePersonagem, listaDeEscudos); }
    void notificarInimigosMaisAgeis() override { TelaCombateRaycaster::notificarInimigosMaisAgeis(); }
    void notificarTurnoExtra(int dexterityJogador, int maxDestrezaInimigos) override { TelaCombateRaycaster::notificarTurnoExtra(dexterityJogador, maxDestrezaInimigos); }
    void notificarDesprevencaoInventario() override { TelaCombateRaycaster::notificarDesprevencaoInventario(); }
    void notificarSemEscudos(const std::string& nomePersonagem) override { TelaCombateRaycaster::notificarSemEscudos(nomePersonagem); }
    void notificarDesequilibrioDefesa(const std::string& nomePersonagem) override { TelaCombateRaycaster::notificarDesequilibrioDefesa(nomePersonagem); }
    void notificarPosturaDefensiva(const std::string& nomePersonagem, const std::string& nomeEscudo) override { TelaCombateRaycaster::notificarPosturaDefensiva(nomePersonagem, nomeEscudo); }
    void notificarAcaoInvalida() override { TelaCombateRaycaster::notificarAcaoInvalida(); }
    void notificarCancelamentoItem() override { TelaCombateRaycaster::notificarCancelamentoItem(); }
    void notificarRequisitoNaoAtendido(const std::string& mensagemRequisito) override { TelaCombateRaycaster::notificarRequisitoNaoAtendido(mensagemRequisito); }
};

class AdaptadorInterfaceDerrota : public IDefeatUI {
public:
    void display(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns) override {
        TelaDerrotaRaycaster::display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns);
    }
};

using DefeatUIAdapter = AdaptadorInterfaceDerrota;

class AdaptadorInterfaceVitoria : public IVictoryUI {
public:
    void display(Character* currentPlayer, int goldEarned, int xpEarned, int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns, const std::vector<std::string>& inimigosDerrotados, int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::pair<std::string, int>>& dropsUnicos, bool podeSubirNivel, const std::vector<std::string>& novasDescobertas, const std::string& tituloMapa) override {
        TelaVitoriaRaycaster::display(currentPlayer, goldEarned, xpEarned, totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns, inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
    }
};

using VictoryUIAdapter = AdaptadorInterfaceVitoria;

class PauseUIAdapter : public IPauseUI {
    int renderizarMenuPause() override { return TelaPauseRaycaster::renderizarMenuPause(); }
    int renderizarMenuConfiguracoes(Character* jogador) override { return TelaPauseRaycaster::renderizarMenuConfiguracoes(jogador); }
    int renderizarMenuAparencia(Character* jogador) override { return TelaPauseRaycaster::renderizarMenuAparencia(jogador); }
    int renderizarMenuFundo(int corFundoAtualIndex) override { return TelaPauseRaycaster::renderizarMenuFundo(corFundoAtualIndex); }
    int renderizarMenuSensibilidade(int percX, int percY) override { return TelaPauseRaycaster::renderizarMenuSensibilidade(percX, percY); }
};

class MapaMundoUIAdapter : public IMapaMundoUI {
    void renderizarPopup(const std::vector<std::string>& arte, const std::vector<std::string>& lugares, int selecao, bool redesenhoCompleto) override { TelaMapaMundoRaycaster::renderizarPopup(arte, lugares, selecao, redesenhoCompleto); }
};

GerenciadorPerspectiva::GerenciadorPerspectiva() : m_visao3DAtiva(true) {
}

void GerenciadorPerspectiva::initialize() {
    m_renderer3D = std::make_unique<RaycasterRendererImpl>();
    m_telas3D = std::make_unique<GerenciadorTelasRaycaster>();
    m_visao3DAtiva = true;
    RendererProvider::set(m_renderer3D.get());
}

void GerenciadorPerspectiva::alternarVisao() {
}

bool GerenciadorPerspectiva::isVisao3DAtiva() const {
    return true;
}

UIRenderer* GerenciadorPerspectiva::obterRendererAtivo() const {
    return m_renderer3D.get();
}

IGerenciadorTelas* GerenciadorPerspectiva::obterGerenciadorTelas() const {
    return m_telas3D.get();
}

float GerenciadorPerspectiva::obterSensibilidadeMouseX() {
    return Raycaster::sensibilidadeX;
}

float GerenciadorPerspectiva::obterSensibilidadeMouseY() {
    return Raycaster::sensibilidadeY;
}

void GerenciadorPerspectiva::definirSensibilidadeMouse(float x, float y) {
    Raycaster::sensibilidadeX = x;
    Raycaster::sensibilidadeY = y;
}

IDiarioUI& GerenciadorPerspectiva::obterDiarioUI() {
    static TelaDiarioRaycaster diarioUI;
    return diarioUI;
}

IInventarioUI& GerenciadorPerspectiva::obterInventarioUI() {
    static TelaInventarioRaycaster inventarioUI;
    return inventarioUI;
}

IAtributosUI& GerenciadorPerspectiva::obterAtributosUI() {
    static AtributosUIAdapter adapter;
    return adapter;
}

IBestiarioUI& GerenciadorPerspectiva::obterBestiarioUI() {
    static BestiarioUIAdapter adapter;
    return adapter;
}

ITelaCombateUI& GerenciadorPerspectiva::obterTelaCombateUI() {
    static TelaCombateUIAdapter adapter;
    return adapter;
}

IDefeatUI& GerenciadorPerspectiva::obterDerrotaUI() {
    static AdaptadorInterfaceDerrota adapter;
    return adapter;
}

IVictoryUI& GerenciadorPerspectiva::obterVitoriaUI() {
    static AdaptadorInterfaceVitoria adapter;
    return adapter;
}

IPauseUI& GerenciadorPerspectiva::obterPauseUI() {
    static PauseUIAdapter adapter;
    return adapter;
}

IMapaMundoUI& GerenciadorPerspectiva::obterMapaMundoUI() {
    static MapaMundoUIAdapter adapter;
    return adapter;
}
