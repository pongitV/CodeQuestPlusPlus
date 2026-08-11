#include "Combat.h"
#include "CombatUIImpl.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <map>
#include <chrono>

#include "../../entities/classes/ClassBase.h"
#include "../inventory/InventoryCombat.h"
#include "../inventory/Item.h"
#include "../inventory/equipment/ShieldEquipment.h"
#include "../../entities/enemies/mahoraga/Mahoraga.h"
#include "../../entities/races/RaceBase.h"
#include "../progress/Bestiary.h"
#include "../progress/Diary.h"
#include "../progress/Progression.h"
#include "../progress/ProgressionFlags.h"
#include "Parry.h"
#include "../../core/utils/RandomGenerator.h"
#include "../../core/utils/InputControl.h"
#include "../../core/state/Debug.h"
#include "../../core/utils/DialogFunctions.h"
#include "./mechanics/DamageCalculator.h"
#include "./mechanics/EnemyMechanics.h"
#include "./mechanics/TurnManager.h"
#include "../../core/utils/Color.h"

#include "../../ui/screens/combat/ScreenCombat.h"

namespace {
    void registrarLog(const std::string& texto, Color /*cor*/ = Color::RESET) {
        if (texto.empty()) return;
        TelaCombate::adicionarMensagemFixa(texto);
    }
}


void Combat::resetarEstatisticasAvancadas() {
    stats_parriesTentados = 0;
    stats_parriesEfetivos = 0;
    stats_parriesPerfeitos = 0;
    stats_maiorDanoCausado = 0;
    stats_itensConsumidos = 0;
    stats_novasDescobertas.clear();
}

Combat::Combat(Character* jogadorParaOCombate,
               std::vector<std::unique_ptr<Character>>&& inimigosParaOCombate,
               std::unique_ptr<ICombateUI> interfaceVisual)
    : currentPlayer(jogadorParaOCombate), listaDeInimigos(std::move(inimigosParaOCombate)), ouroObtido(0), xpObtido(0), totalDamageDealt(0), totalDamageTaken(0), contadorDoTurnoAtual(1),
      ui(interfaceVisual ? std::move(interfaceVisual) : std::make_unique<CombateUIImpl>())
{

    int nivelDeDificuldade = static_cast<int>(currentPlayer->obterDificuldade());
    double multiplicadorDeDificuldadeDosInimigos = 1.0;

    if (nivelDeDificuldade == 2) {
        multiplicadorDeDificuldadeDosInimigos = 1.5;
    } else if (nivelDeDificuldade == 3) {
        multiplicadorDeDificuldadeDosInimigos = 2.0;
    }

    for (auto& inimigoAtualPtr : this->listaDeInimigos) 
    {
        inimigoAtualPtr->aplicarMultiplicadorDificuldade(multiplicadorDeDificuldadeDosInimigos);
        inimigoAtualPtr->prepararParaNovaBatalha();
    }
}

void Combat::setContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    if (ui) {
        ui->configurarContexto3D(modo3D, matriz, posX, posY, angulo, titulo);
    }
}

void Combat::adicionarAliados(std::vector<std::unique_ptr<Character>> aliados)
{
    listaDeAliados = std::move(aliados);
}

void Combat::adicionarAliadoEmCombate(std::unique_ptr<Character> aliado) {
    listaDeAliados.push_back(std::move(aliado));
}

Combat::~Combat()
{
    Parry::onUpdateScreen = nullptr;
}

std::string Combat::obterTituloDoCombate() const
{
    std::string titulo = "EM COMBATE (";
    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
        titulo += listaDeInimigos[i]->getName();
        if (i < listaDeInimigos.size() - 1) titulo += ", ";
    }
    titulo += ")";
    return titulo;
}

bool Combat::ehPersonagemJogadorOuAliado(Character* character) const {
    if (character == currentPlayer) return true;
    for (const auto& aliadoAtual : listaDeAliados) {
        if (aliadoAtual.get() == character) return true;
    }
    return false;
}

std::vector<Character*> Combat::obterInimigosRaw() const
{
    std::vector<Character*> ponteirosInimigos(listaDeInimigos.size());
    std::transform(listaDeInimigos.begin(), listaDeInimigos.end(), ponteirosInimigos.begin(), [](const std::unique_ptr<Character>& ptr) { return ptr.get(); });
    return ponteirosInimigos;
}

void Combat::displayTelaDeCombate(bool animarEntrada) const
{
    ui->atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), currentPlayer, obterAliadosVivosRaw(), animarEntrada);
}

std::vector<Character*> Combat::obterAliadosVivosRaw() const {
    std::vector<Character*> aliadosVivos;
    for (const auto& aliado : listaDeAliados) {
        if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
    }
    return aliadosVivos;
}

std::string Combat::getNameFormatadoComNumero(Character* c) const {
    if (!c) return "Desconhecido";
    if (c == currentPlayer) return c->getName();
    for (size_t i = 0; i < listaDeAliados.size(); ++i) {
        if (listaDeAliados[i].get() == c) {
            return c->getName() + " (Aliado " + std::to_string(i + 1) + ")";
        }
    }
    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
        if (listaDeInimigos[i].get() == c) {
            return c->getName() + " (" + std::to_string(i + 1) + ")";
        }
    }
    return c->getName();
}

void Combat::prepararTurnoPersonagem(Character* character) {
    std::string nomeChar = getNameFormatadoComNumero(character);
    registrarLog("");
    registrarLog("=== TURNO " + std::to_string(contadorDoTurnoAtual) + " | VEZ DE " + nomeChar + " ===");
    ui->definirTurnoVisivel(contadorDoTurnoAtual, nomeChar);
    character->reduzirCooldowns();
    character->processarEfeitosInicioTurno();
}



bool Combat::executarTurnoJogadorOuAliado(Character* character, bool& primeiraRenderizacao, bool processarEfeitosInicio) {
    if (processarEfeitosInicio) {
        prepararTurnoPersonagem(character);
    }
    if (character->obterVida() <= 0) return false;

    if (character == currentPlayer) {
        bool limpouAliado = false;
        for (auto& aliado : listaDeAliados) {
            if (aliado->isMinion() && aliado->obterVida() > 0) {
                int damage = std::max(1, static_cast<int>(aliado->obterVidaMaxima() * 0.15));
                aliado->modificarVida(-damage);
                registrarLog(DialogFunctions::formatarMsgStatus(aliado->getName() + " perdeu " + std::to_string(damage) + " HP (decomposicao).", Color::MAGENTA));
                
                if (aliado->obterVida() <= 0) {
                    registrarLog(DialogFunctions::formatarMsgStatus(aliado->getName() + " se decompos durante o combat", Color::RED));
                    limpouAliado = true;
                }
            }
        }
        // Remove definitivamente da memória os aliados que morreram pelo dreno
        if (limpouAliado) {
            listaDeAliados.erase(std::remove_if(listaDeAliados.begin(), listaDeAliados.end(), [](const auto& a) { return a->obterVida() <= 0; }), listaDeAliados.end());
        }
    }

    bool turnoConsumido = false;
    bool usouInventario = false;

    while (!turnoConsumido && character->obterVida() > 0 && !listaDeInimigos.empty()) {
        displayTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        processarMenuDeAcoesDoJogador(character, turnoConsumido, usouInventario);
        
        limparInimigosMortos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return true; 
    }

    if (usouInventario) {
        displayTelaDeCombate();
        ui->notificarDesprevencaoInventario();
    }
    return false;
}

void Combat::iniciarCombate() 
{
    Parry::onUpdateScreen = [this]() {
        this->displayTelaDeCombate(false);
    };
    resetarEstatisticasAvancadas();
    currentPlayer->prepararParaNovaBatalha();
    ui->limparMensagensFixas();

    for (auto& aliado : listaDeAliados) {
        aliado->prepararParaNovaBatalha();
    }
    ui->animarIntroducaoCombate(obterTituloDoCombate(), obterInimigosRaw(), currentPlayer);

    ui->limparTela();

    int maxDestrezaInimigos = GerenciadorTurnos::calcularMaxDestrezaInimigos(listaDeInimigos);
    for (const auto& inimigoPtr : listaDeInimigos) {
        Bestiary::instancia().registrarPrimeiraVista(inimigoPtr->obterRaca()->getRaceName());
        Diary::instancia().registrarRaca(inimigoPtr->obterRaca()->getRaceName());
        if (inimigoPtr->getNameClasse() != "Monstro") {
            Diary::instancia().registrarClasse(inimigoPtr->getNameClasse());
        }
    }
    
    bool turnoExtraFirstTurn = GerenciadorTurnos::jogadorTemTurnoExtraNoInicio(currentPlayer, maxDestrezaInimigos);
    bool primeiraRenderizacao = false; // Modificado, pois ja animamos na intro
    
    if (GerenciadorTurnos::inimigosSaoMaisAgeis(currentPlayer, maxDestrezaInimigos)) {
        displayTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        
        if (GerenciadorTurnos::inimigosTemDobroDeAgilidade(currentPlayer, maxDestrezaInimigos)) {
            std::string alert = "A agilidade extrema dos enemies (" + std::to_string(maxDestrezaInimigos) + " VS " + std::to_string(currentPlayer->getDexterity()) + ") permite que eles ataquem duas vezes seguidas!";
            InputControl::lerSelecaoMenuEmPopup("ALERTA DE AGILIDADE", {alert}, {"OK"}, Color::RED);

            executarTurnoDeTodosOsInimigos();
            limparInimigosMortos();
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
            executarTurnoDeTodosOsInimigos();
            limparInimigosMortos();
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
            
            contadorDoTurnoAtual++; // Jogador comeca no Turno 2
        } else {
            ui->notificarInimigosMaisAgeis();
            executarTurnoDeTodosOsInimigos();
            limparInimigosMortos();
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
        }
    }

    while (currentPlayer->obterVida() > 0 && !listaDeInimigos.empty()) {
        // Turno do Jogador
        if (currentPlayer->obterVida() > 0) {
            if (executarTurnoJogadorOuAliado(currentPlayer, primeiraRenderizacao)) return;

            if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) {
                ui->notificarTurnoExtra(currentPlayer->getDexterity(), maxDestrezaInimigos);
                turnoExtraFirstTurn = false;
                if (executarTurnoJogadorOuAliado(currentPlayer, primeiraRenderizacao, false)) return;
            }
        }
        
        // Turnos dos Aliados
        for (size_t i = 0; i < listaDeAliados.size(); ++i) {
            Character* aliado = listaDeAliados[i].get();
            if (aliado->obterVida() <= 0 || listaDeInimigos.empty()) continue;
            
            bool isPrimeiraRend = false;
            if (executarTurnoJogadorOuAliado(aliado, isPrimeiraRend)) return;
        }
        
        executarTurnoDeTodosOsInimigos();
        limparInimigosMortos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;

        contadorDoTurnoAtual++;
    }
}

void Combat::processarMenuDeAcoesDoJogador(Character* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int acaoEscolhida = ui->obterAcaoDoJogador(contadorDoTurnoAtual, personagemAgindo, obterInimigosRaw(), currentPlayer, obterAliadosVivosRaw());
    
    ui->limparContextoPersonagemHUD(); // Forca reset visual ao retornar para evitar bugs de persistencia de interface

    switch (acaoEscolhida) 
    {
        case 1: processarAcaoAtacar(personagemAgindo, turnoFoiConsumido); break;
        case 2: processarAcaoDefender(personagemAgindo, turnoFoiConsumido); break;
        case 3: processarAcaoHabilidade(personagemAgindo, turnoFoiConsumido); break;
        case 4: processarAcaoInventario(personagemAgindo, turnoFoiConsumido, usouInventarioNoTurno); break;
        case 5: ui->displayTelaAtributos(personagemAgindo); break;
        case 6: ui->displayTelaDiario(personagemAgindo); break;
        case 7: break;
        default: 
            ui->notificarAcaoInvalida();
            break;
    }
}

void Combat::processarAcaoAtacar(Character* personagemAgindo, bool& turnoFoiConsumido)
{
    std::string nomeArma = personagemAgindo->obterArma() ? (" com " + personagemAgindo->obterArma()->getNameItem()) : "";

    if (personagemAgindo->getAttackType() == TipoAtaque::AREA) 
    {
        registrarLog(personagemAgindo->getName() + " desferiu um ataque em ÁREA" + nomeArma + "!");
        realizarAtaqueFisico(personagemAgindo, nullptr, contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }
    else 
    {
        int indiceDoAlvoEscolhido = ui->obterAlvoAtaque(obterTituloDoCombate(), obterInimigosRaw(), currentPlayer, obterAliadosVivosRaw());
        if (indiceDoAlvoEscolhido == -1) return;

        Character* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
        std::string nomeAlvo = getNameFormatadoComNumero(alvo);
        registrarLog(personagemAgindo->getName() + " iniciou ataque em " + nomeAlvo + nomeArma + "!");

        realizarAtaqueFisico(personagemAgindo, alvo, contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }
}

Item* Combat::selecionarEscudo(Character* personagemAgindo) 
{
    std::vector<Item*> listaDeEscudos;
    for (auto* item : personagemAgindo->obterInventario()->obterTodosOsItens()) 
    {
        if (item->obterTipo() == TipoEquipamento::ESCUDO) {
            listaDeEscudos.push_back(item);
        }
    }

    if (listaDeEscudos.empty()) 
    {
        ui->notificarSemEscudos(personagemAgindo->getName());
        return nullptr;
    }

    int opcaoEscolhida = ui->obterEscolhaDeEscudo(personagemAgindo->getName(), listaDeEscudos);
    return (opcaoEscolhida == 0) ? nullptr : listaDeEscudos[opcaoEscolhida - 1];
}

void Combat::processarAcaoDefender(Character* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterRecargaDefesa()) 
    {
        ui->notificarDesequilibrioDefesa(personagemAgindo->getName());
        return; 
    }
    
    Item* escudoEscolhido = selecionarEscudo(personagemAgindo);
    if (escudoEscolhido != nullptr) 
    {
        if (escudoEscolhido->obterDurabilidadeAtualEscudo() <= 0) {
            std::string alert = "O escudo [" + escudoEscolhido->getNameItem() + "] esta quebrado e nao pode ser usado!";
            InputControl::lerSelecaoMenuEmPopup("ESCUDO QUEBRADO", {alert}, {"OK"}, Color::RED);
            return; // Nao consome o turno
        }

        if (!escudoEscolhido->podeSerEquipadoPor(personagemAgindo)) {
            ui->notificarRequisitoNaoAtendido(escudoEscolhido->obterMensagemRequisito());
            return;
        }

        personagemAgindo->equiparItem(escudoEscolhido);
        personagemAgindo->definirDefendendo(true);
        std::string msgDefensa = personagemAgindo->getName() + " ASSUME POSTURA DEFENSIVA COM " + escudoEscolhido->getNameItem();
        registrarLog(msgDefensa);
        ui->definirMensagemBanner(msgDefensa, CorBanner::YELLOW);
        ui->notificarPosturaDefensiva(personagemAgindo->getName(), escudoEscolhido->getNameItem());
        turnoFoiConsumido = true;
    }
}

void Combat::processarAcaoHabilidade(Character* personagemAgindo, bool& turnoFoiConsumido)
{
    std::vector<Character*> alvosRaw = obterInimigosRaw();
    
    personagemAgindo->definirHabilidadeCancelada(false);
    std::string msgHab = personagemAgindo->getName() + " USA HABILIDADE: " + personagemAgindo->getNameClasse();
    registrarLog(msgHab);
    ui->definirMensagemBanner(msgHab, CorBanner::YELLOW);
    personagemAgindo->obterClasse()->useClassAbility(this, personagemAgindo, alvosRaw);
    
    if (personagemAgindo->obterHabilidadeCancelada()) return;

    if (personagemAgindo->habilidadeDaClasseConsomeTurno()) turnoFoiConsumido = true;
    else {
        // Não bloqueia thread - deixa o game loop continuar
        InputControl::limparBuffer();
    }
}

void Combat::processarAcaoInventario(Character* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int vidaAntes = personagemAgindo->obterVida();
    bool inventarioConsumiu = false;
    
    InventarioCombate::gerenciarInventario(personagemAgindo, &inventarioConsumiu);
    if (inventarioConsumiu) {
        turnoFoiConsumido = true;
        usouInventarioNoTurno = true;
    }
    
    if (personagemAgindo->obterVida() > vidaAntes) {
        int cura = personagemAgindo->obterVida() - vidaAntes;
        std::string msgCura = personagemAgindo->getName() + " SE CUROU (+" + std::to_string(cura) + " HP)";
        registrarLog(msgCura);
        ui->definirMensagemBanner(msgCura, CorBanner::GREEN_CLARO);
        ui->animarCuraNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAgindo, currentPlayer, obterAliadosVivosRaw(), cura);
    }

    if (personagemAgindo->obterItemSelecionadoParaUso() != nullptr) 
    {
        Item* itemSelecionado = personagemAgindo->obterItemSelecionadoParaUso();
        
        int indiceDoAlvoEscolhido = ui->obterAlvoItem(obterTituloDoCombate(), obterInimigosRaw(), currentPlayer, obterAliadosVivosRaw());

        if (indiceDoAlvoEscolhido == -1) 
        {
            ui->notificarCancelamentoItem();
            personagemAgindo->definirItemSelecionadoParaUso(nullptr);
        } 
        else 
        {
            Character* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
            std::string nomeAlvo = getNameFormatadoComNumero(alvo);
            std::string msgUsoItem = personagemAgindo->getName() + " USA " + itemSelecionado->getNameItem() + " EM " + nomeAlvo;
            registrarLog(msgUsoItem);
            ui->definirMensagemBanner(msgUsoItem, CorBanner::GREEN_CLARO);
            
            itemSelecionado->usar(personagemAgindo, alvo);
            
            if (personagemAgindo->obterConsumivelRapido() == itemSelecionado) {
                personagemAgindo->desequiparConsumivel();
                std::string nomeDesteItem = itemSelecionado->getNameItem();
                for (auto* outroItem : personagemAgindo->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != itemSelecionado && outroItem->getNameItem() == nomeDesteItem) {
                        personagemAgindo->equiparItem(outroItem);
                        break;
                    }
                }
            }
            
            personagemAgindo->obterInventario()->removerItem(itemSelecionado);
            personagemAgindo->definirItemSelecionadoParaUso(nullptr);
            turnoFoiConsumido = true;
            usouInventarioNoTurno = true;
            stats_itensConsumidos++;
        }
    }
}

void Combat::limparInimigosMortos()
{
    for (auto& inimigoPtr : listaDeInimigos) 
    {
        if (inimigoPtr->obterVida() <= 0) 
        {
                int xpAntes = xpObtido;
                int ouroAntes = ouroObtido;
                size_t itensAntes = itensObtidos.size();

                std::string nomeInimigoMorto = getNameFormatadoComNumero(inimigoPtr.get());
                processarMorteDeInimigo(inimigoPtr.get());

                int xpDrop = xpObtido - xpAntes;
                int ouroDrop = ouroObtido - ouroAntes;
                
                std::string msgMorte = nomeInimigoMorto + " E DERROTADO! (+" + std::to_string(xpDrop) + " XP, +" + std::to_string(ouroDrop) + " G)";
                registrarLog(msgMorte);
                ui->definirMensagemBanner(msgMorte, CorBanner::OURO);
                
                std::vector<std::string> dropsDaMorte;
                if (xpDrop > 0) dropsDaMorte.push_back("+" + std::to_string(xpDrop) + " XP");
                if (ouroDrop > 0) dropsDaMorte.push_back("+" + std::to_string(ouroDrop) + "G");
                
                std::map<std::string, int> contagemItens;
                for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
                    contagemItens[itensObtidos[i]]++;
                }
                for (auto const& [nome, qtd] : contagemItens) {
                    dropsDaMorte.push_back("+" + std::to_string(qtd) + "x " + nome);
                }

                if (!dropsDaMorte.empty()) {
                    registrarLog("Recompensas de " + nomeInimigoMorto + ":");
                    for (size_t d = 0; d < dropsDaMorte.size(); ++d) {
                        registrarLog("  - " + dropsDaMorte[d]);
                    }
                }



                std::vector<Character*> aliadosVivos = obterAliadosVivosRaw();
                ui->animarMorteInimigo(obterTituloDoCombate(), obterInimigosRaw(), inimigoPtr.get(), currentPlayer, aliadosVivos, dropsDaMorte);
                inimigoPtr->definirMorteAnimada(true);
                ui->limparContextoInimigoMortoEDrops();
        }
    }

    listaDeInimigos.erase(std::remove_if(listaDeInimigos.begin(), listaDeInimigos.end(), [](const auto& enemy) { return enemy->obterVida() <= 0; }), listaDeInimigos.end());
}


void Combat::executarTurnoDeTodosOsInimigos() 
{
    ui->limparMensagensFixas();
    if (currentPlayer->obterPularTurnoInimigo()) 
    {
        // A mensagem na UI foi removida para priorizar o combat limpo
        registrarLog(DialogFunctions::formatarMsgStatus("Os inimigos estao atordoados e nao podem agir!", Color::GREEN));
        currentPlayer->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::string textoTurnoInimigos = "═══ TURNO " + std::to_string(contadorDoTurnoAtual) + " ║ VEZ DOS INIMIGOS ═══";
        registrarLog("");
        registrarLog(textoTurnoInimigos);
        ui->definirTurnoVisivel(contadorDoTurnoAtual, "INIMIGOS");
        ui->definirMensagemBanner("VEZ DOS INIMIGOS INICIA - PRESSIONE ENTER", CorBanner::ORANGE);
        displayTelaDeCombate(false); // Forca o HUD a atualizar o nome do Turno para os enemies antes do ataque iniciar
        InputControl::limparBuffer();
        InputControl::aguardarEnter("Vez dos inimigos inicia. Pressione ENTER para continuar...");

        for (size_t i = 0; i < listaDeInimigos.size(); ++i) 
        {
            auto& inimigoAtualPtr = listaDeInimigos[i];
            if (currentPlayer->obterVida() <= 0) break; // Interrompe se o jogador morrer
            
            Character* inimigoAtual = inimigoAtualPtr.get();
            if (!inimigoAtual || inimigoAtual->obterVida() <= 0) {
                continue;
            }

            Parry::definirInimigoAtacante(inimigoAtual);
            inimigoAtual->processarEfeitosInicioTurno();
            if (inimigoAtual->obterVida() <= 0) {
                Parry::definirInimigoAtacante(nullptr);
                continue;
            }

            std::string motivoIncapacidade;
            if (inimigoAtual->podeAgir(motivoIncapacidade)) 
            {
                // Logica de escolha de alvo do enemy
                Character* alvo = MecanicasInimigo::escolherAlvo(obterAliadosVivosRaw(), currentPlayer);

                bool turnoConsumidoPorHabilidade = inimigoAtual->obterRaca()->tentarUsarHabilidadeAtiva(inimigoAtual, alvo, static_cast<int>(currentPlayer->obterDificuldade()));
                
                if (!turnoConsumidoPorHabilidade) {
                    realizarAtaqueFisico(inimigoAtual, alvo, contadorDoTurnoAtual);
                }
            }
            else
            {
                // A mensagem na UI foi removida para priorizar o combat limpo
                registrarLog(DialogFunctions::formatarMsgStatus(inimigoAtual->getName() + " esta sob efeito de " + motivoIncapacidade + " e nao pode agir!", Color::GREEN));
            }
            Parry::definirInimigoAtacante(nullptr);
        }
    }

    if (currentPlayer->obterDefendendo())
    {
        currentPlayer->definirDefendendo(false);
        currentPlayer->definirRecargaDefesa(true);
    }
    else if (currentPlayer->obterRecargaDefesa())
    {
        currentPlayer->definirRecargaDefesa(false);
    }

    if (currentPlayer->obterRecarga()) currentPlayer->definirRecarga(false);
    InputControl::limparBuffer();
    ui->definirMensagemBanner("TURNO DOS INIMIGOS FINALIZADO - PRESSIONE ENTER", CorBanner::OURO);
    InputControl::aguardarEnter("Turno dos inimigos finalizado. Pressione ENTER para o seu turno!");
    ui->definirMensagemBanner("TURNO DO JOGADOR - ESCOLHA UMA ACAO", CorBanner::OURO);
}

void Combat::realizarAtaqueFisico(Character* personagemAtacante, Character* personagemDefensor, int turnoAtualDoCombate) 
{
    auto [baseDamageCalculado, danoPerfurante] = CalculadoraDano::calcularDanoOfensivoBase(personagemAtacante);

    bool isAtacanteJogadorOuAliado = ehPersonagemJogadorOuAliado(personagemAtacante);

    if (isAtacanteJogadorOuAliado || static_cast<int>(currentPlayer->obterDificuldade()) >= 2) 
    {
        baseDamageCalculado = personagemAtacante->obterRaca()->processOffensiveDamage(baseDamageCalculado, personagemAtacante);
    }

    auto callbackAplicarDano = [this, turnoAtualDoCombate](Character* atacante, Character* alvo, int danoBruto, int perfurante) {
        this->applyDamageAoAlvo(atacante, alvo, danoBruto, perfurante, turnoAtualDoCombate);
    };

    bool aplicarPassivaClasse = isAtacanteJogadorOuAliado || static_cast<int>(currentPlayer->obterDificuldade()) == 3;

    personagemAtacante->obterClasse()->executarAtaqueComPassivaDaClasse(personagemAtacante, personagemDefensor, baseDamageCalculado, danoPerfurante, listaDeInimigos, callbackAplicarDano, aplicarPassivaClasse);
}



void Combat::processarPosDano(Character* atacante, Character* alvo, int finalDamage, bool tentouParry, bool parrySucesso) {
    std::vector<Character*> aliadosVivos = obterAliadosVivosRaw();

    Parry::definirInimigoAtacante(atacante);
    Parry::definirParryStatus(0);
    if (tentouParry) {
        if (parrySucesso) {
            if (finalDamage <= 0) Parry::definirParryStatus(1);
            else Parry::definirParryStatus(2);
        } else {
            Parry::definirParryStatus(3);
        }
    }

    std::string msgAtaqueDano = "";
    std::string msgParryLinha2 = "";
    CorBanner corBanner = CorBanner::OURO;

    if (ehPersonagemJogadorOuAliado(atacante)) {
        msgAtaqueDano = atacante->getName() + " ATACA " + getNameFormatadoComNumero(alvo) + " (" + std::to_string(finalDamage) + " DE DANO) - PRESSIONE ENTER";
        corBanner = CorBanner::YELLOW;
    } else {
        msgAtaqueDano = getNameFormatadoComNumero(atacante) + " ATACA JOGADOR (" + std::to_string(finalDamage) + " DE DANO) - PRESSIONE ENTER";
        corBanner = CorBanner::ORANGE;

        if (tentouParry) {
            if (parrySucesso) {
                int danoRefletido = (finalDamage <= 0) ? std::max(1, atacante->getStrength() / 2) : 0;
                Parry::definirUltimoDanoRefletido(danoRefletido);
                if (finalDamage <= 0) {
                    msgParryLinha2 = "PARRY PERFEITO! - DANO REFLETIDO: " + std::to_string(danoRefletido);
                } else {
                    msgParryLinha2 = "PARRY EFETIVO! - DANO REDUZIDO";
                }
            } else {
                msgParryLinha2 = "PARRY FALHOU!";
            }
        }
    }
    ui->definirMensagemBanner(msgAtaqueDano, corBanner, msgParryLinha2);

    if (finalDamage > 0) 
    {
        // ANIMACAO DO DANO NO INIMIGO (Piscar Vermelho + Flicker)
        if (!ehPersonagemJogadorOuAliado(alvo)) {
            ui->animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), alvo, atacante, currentPlayer, aliadosVivos, finalDamage);
        }
        else {
            ui->animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), alvo, currentPlayer, aliadosVivos, false, finalDamage);
        }

        // Aplicação dos efeitos no acerto
        int vidaAtacanteAntes = atacante->obterVida();
        
        if (atacante->obterArma()) {
            atacante->obterArma()->aoCausarDano(atacante, alvo, finalDamage);
        }
        atacante->obterRaca()->aoCausarDano(atacante, alvo, finalDamage);
        
        // Verifica se o atacante se curou (Ex: Passiva da Abominacao)
        if (atacante->obterVida() > vidaAtacanteAntes) {
            int curaInimigo = atacante->obterVida() - vidaAtacanteAntes;
            if (!ehPersonagemJogadorOuAliado(atacante)) {
                ui->definirMensagemBanner(getNameFormatadoComNumero(atacante) + " SE CUROU (" + std::to_string(curaInimigo) + " HP)", CorBanner::GREEN_ESCURO);
                ui->animarCuraNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), atacante, currentPlayer, aliadosVivos, curaInimigo);
            } else {
                ui->definirMensagemBanner(atacante->getName() + " SE CUROU (" + std::to_string(curaInimigo) + " HP)", CorBanner::GREEN_CLARO);
                ui->animarCuraNoJogador(obterTituloDoCombate(), obterInimigosRaw(), atacante, currentPlayer, aliadosVivos, curaInimigo);
            }
        }
        
        if (alvo->obterArmadura() && alvo->obterArmadura()->temPropriedade(Propriedade::ArmaduraAdaptacao)) {
            auto* ef = const_cast<EfeitoStatus*>(alvo->encontrarEfeito(EfeitoID::RodaAdaptacao));
            if (ef) {
                auto* efRoda = dynamic_cast<EfeitoRodaAdaptacao*>(ef);
                if (efRoda) efRoda->adaptar(alvo, atacante);
            }
        }
    }
    else if (tentouParry && parrySucesso && ehPersonagemJogadorOuAliado(alvo)) {
        ui->animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), alvo, currentPlayer, aliadosVivos, true, finalDamage);
    } else {
        ui->atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), currentPlayer, aliadosVivos);
        // Não bloqueia thread - deixa o game loop continuar
        InputControl::limparBuffer();
    }

    Parry::definirParryStatus(0);

    limparInimigosMortos();
}

void Combat::applyDamageAoAlvo(Character* personagemAtacante, Character* personagemAlvo, int danoBruto, int danoPerfurante, int /*turnoAtualDoCombate*/) 
{
    if (Debug::isOneHitKillActive && personagemAtacante == currentPlayer) {
        danoBruto = DANO_MAXIMO_DEBUG;
    }
    if (Debug::isGodModeActive && personagemAlvo == currentPlayer) {
        danoBruto = DANO_NULO;
        danoPerfurante = DANO_NULO;
    }
    if (personagemAlvo->possuiEfeito(EfeitoID::Inviolavel))
    {
        std::string msgEsquiva = personagemAlvo->getName() + " evitou o ataque de " + personagemAtacante->getName();
        registrarLog(DialogFunctions::formatarMsgCombate(msgEsquiva, Color::CYAN));
        
        std::vector<Character*> aliadosVivos = obterAliadosVivosRaw();
        ui->atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), currentPlayer, aliadosVivos);
        // Não bloqueia thread - deixa o game loop continuar
        InputControl::limparBuffer();
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante->obterArma()) personagemAtacante->obterArma()->antesDeCausarDano(personagemAtacante, personagemAlvo);

    int baseDamageMitigado = CalculadoraDano::calcularMitigacaoDefensiva(personagemAlvo, danoBruto, danoPerfurante);
    int danoReduzidoPeloParry = 0;
    bool tentouParry = false;
    bool parryFoiBemSucedido = false;
    
    bool ataqueImparavel = personagemAtacante && personagemAtacante->obterRaca()->ignoraParry();

    // Logica do Parry (apenas quando o inimigo ataca o jogador/aliado)
    if (ehPersonagemJogadorOuAliado(personagemAlvo) && !ehPersonagemJogadorOuAliado(personagemAtacante) && personagemAlvo->obterParryAtivado() && !personagemAlvo->obterDefendendo()) 
    {
        if (ataqueImparavel) {
            std::string msgImparavel = DialogFunctions::formatarMsgCombate(personagemAtacante->getName() + " desfere um ATAQUE IMPARAVEL! O Parry foi ignorado!", Color::FUNDO_RED);
            registrarLog(msgImparavel);
            ui->adicionarMensagemFixa(ui->margemCombate() + msgImparavel + "\n");
        } else {
            tentouParry = true;
            parryFoiBemSucedido = Parry::tentarParry(personagemAtacante, personagemAlvo, baseDamageMitigado, danoReduzidoPeloParry);
            stats_parriesTentados++;
            if (parryFoiBemSucedido) stats_parriesEfetivos++;
        }
    }

    bool aplicarPassivas = (ehPersonagemJogadorOuAliado(personagemAlvo) || static_cast<int>(currentPlayer->obterDificuldade()) >= 2);

    ResultadoDano res = personagemAlvo->receberDano(danoBruto, danoPerfurante, danoReduzidoPeloParry, personagemAtacante, aplicarPassivas);

    // Logica de adaptacao do Mahoraga ao ter seu ataque bloqueado por escudo
    if (res.danoBloqueado > 0 && personagemAtacante->obterRaceType() == RaceType::Mahoraga) {
        // Precisamos de um cast para chamar o metodo especifico da race Mahoraga
        auto* mahoraga = dynamic_cast<Mahoraga*>(personagemAtacante->obterRaca());
        if (mahoraga) {
            mahoraga->aoTerAtaqueBloqueadoPorEscudo();
        }
    }

    // Burlar o limite de "minimo de 1 de damage" do system base caso o Parry absorva todo o impacto
    if (tentouParry && parryFoiBemSucedido && danoReduzidoPeloParry >= baseDamageMitigado) 
    {
        if (personagemAlvo == currentPlayer) stats_parriesPerfeitos++;
        if (res.finalDamage > 0) 
        {
            personagemAlvo->modificarVida(res.finalDamage); // Restaura o HP retirado pela trava de minimo de damage
            res.finalDamage = 0; // Anula o damage para ativar a Reflexao de Parry Perfeito
        }
    }

    displayResultadoDoAtaque(personagemAtacante, personagemAlvo, res.finalDamage, tentouParry, parryFoiBemSucedido, res.danoBloqueado, res.escudoQuebrou, res.nomeEscudoQuebrado);

    processarPosDano(personagemAtacante, personagemAlvo, res.finalDamage, tentouParry, parryFoiBemSucedido);

    if (tentouParry && parryFoiBemSucedido && res.finalDamage <= 0 && ehPersonagemJogadorOuAliado(personagemAlvo) && personagemAtacante) {
        personagemAtacante->obterRaca()->aoSofrerParryPerfeito();

        int danoRefletido = std::max(1, (danoBruto + danoPerfurante) / 2);
        personagemAtacante->modificarVida(-danoRefletido);
        std::string atacanteReflexao = getNameFormatadoComNumero(personagemAtacante);
        
        std::string msgReflexao = "PARRY PERFEITO! " + getNameFormatadoComNumero(personagemAlvo) + " refletiu " + std::to_string(danoRefletido) + " de dano de volta em " + atacanteReflexao + "!";
        registrarLog(msgReflexao);
        
        std::vector<Character*> aliadosVivos = obterAliadosVivosRaw();
        if (!ehPersonagemJogadorOuAliado(personagemAtacante)) {
            ui->animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), personagemAtacante, personagemAlvo, currentPlayer, aliadosVivos, danoRefletido);
            totalDamageDealt += danoRefletido;
        } else {
            ui->animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAtacante, currentPlayer, aliadosVivos, false, danoRefletido);
        }
    }
}

void Combat::displayResultadoDoAtaque(Character* atacante, Character* alvo, int finalDamage, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado)
{
    bool isJogadorOuAliado = ehPersonagemJogadorOuAliado(alvo);
    std::string nomeAtacante = getNameFormatadoComNumero(atacante);
    std::string nomeAlvo = getNameFormatadoComNumero(alvo);

    if (danoBloqueado > 0) {
        std::string msgDefesa = "O escudo de " + nomeAlvo + " bloqueou " + std::to_string(danoBloqueado) + " de dano do ataque de " + nomeAtacante + "!";
        registrarLog(msgDefesa);
        
        if (escudoQuebrou) {
            std::string msgQuebra = "ALERTA: O escudo [" + nomeEscudoQuebrado + "] de " + nomeAlvo + " FOI DESTRUIDO pelo impacto!";
            registrarLog(msgQuebra);
            alvo->desequiparEscudo();
        }
    }

    if (isJogadorOuAliado) 
    {
        if (tentouParry) {
            std::string mensagemParryLog = Parry::obterMensagemFeedback(parrySucesso, finalDamage);
            registrarLog(nomeAlvo + " realizou PARRY contra " + nomeAtacante + ": " + mensagemParryLog);
        }
        else if (finalDamage > 0) 
        {
            registrarLog(nomeAtacante + " atacou " + nomeAlvo + " e causou " + std::to_string(finalDamage) + " de dano!");
        }
        else if (finalDamage == 0 && alvo->obterDefendendo()) 
        {
            registrarLog("O dano do ataque de " + nomeAtacante + " foi totalmente absorvido pela defesa de " + nomeAlvo + "!");
        }

        
        if (finalDamage > 0 && alvo == currentPlayer) totalDamageTaken += finalDamage;
    }
    else 
    {
        if (finalDamage > 0) {
            registrarLog(nomeAtacante + " atacou " + nomeAlvo + " e causou " + std::to_string(finalDamage) + " de dano!");
            if (finalDamage > stats_maiorDanoCausado) stats_maiorDanoCausado = finalDamage;
            if (alvo != currentPlayer) totalDamageDealt += finalDamage;
        } else if (finalDamage == 0 && alvo->obterDefendendo()) {
            registrarLog(nomeAlvo + " bloqueou completamente o dano de " + nomeAtacante + "!");
        }
    }
}

bool Combat::verificarCondicaoDeVitoriaOuDerrota() 
{
    bool isVitoria = listaDeInimigos.empty();
    bool isDerrota = currentPlayer->obterVida() <= 0;

    if (isVitoria || isDerrota) 
    { 
        currentPlayer->limparEfeitos(); // Remove buffs e debuffs ao final da batalha
        if (isVitoria) {
            ui->displayTelaVitoria(currentPlayer, ouroObtido, xpObtido, totalDamageDealt, 
                                totalDamageTaken, currentPlayer->obterCuraTotalRecebida(), contadorDoTurnoAtual, 
                                itensObtidos, inimigosDerrotados, stats_parriesPerfeitos, stats_maiorDanoCausado, stats_parriesTentados, stats_parriesEfetivos, stats_itensConsumidos, stats_novasDescobertas);
        } else {
            ui->displayTelaDerrota(currentPlayer, ouroObtido, xpObtido, totalDamageDealt, totalDamageTaken, currentPlayer->obterCuraTotalRecebida(), contadorDoTurnoAtual); 
        }
        currentPlayer->finalizarBatalha();
        return true; 
    }
    return false;
}

void Combat::processarMorteDeInimigo(Character* enemy)
{
    registrarLog(DialogFunctions::formatarMsgCombate(enemy->getName() + " derrotado!", Color::RED));
    inimigosDerrotados.push_back(enemy->getName());

    std::string nomeRaca = enemy->obterRaca()->getRaceName();
    if (!Bestiary::instancia().jaDerrotado(nomeRaca)) {
        stats_novasDescobertas.push_back("Novo monstro catalogado: " + nomeRaca);
    }

    Bestiary::instancia().registrarDerrota(enemy->obterRaca()->getRaceName());

    if (enemy->getName() == "Mahoraga") {
        Progression::instancia().definirFlag(Flags::Floresta_MahoragaDerrotado, true);
    }

    // Passiva do Necromancer: Coletar alma
    if (currentPlayer->obterClassType() == ClassType::NECROMANTE) {
        currentPlayer->adicionarAlma(enemy->clone());
        std::string msg = DialogFunctions::formatarMsgHabilidade("Voce coletou a alma de " + enemy->getName() + "!", Color::MAGENTA);
        registrarLog(msg);
    }

    registrarLog("═══ DROPS ═══", Color::YELLOW);

    size_t itensAntes = itensObtidos.size();
    enemy->executarDrops(currentPlayer, itensObtidos, ouroObtido, xpObtido);
    for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
        if (!Bestiary::instancia().jaColetouDrop(nomeRaca, itensObtidos[i])) {
            stats_novasDescobertas.push_back("Novo drop descoberto: " + itensObtidos[i]);
        }
        Bestiary::instancia().registrarDrop(enemy->obterRaca()->getRaceName(), itensObtidos[i]);
    }
}
