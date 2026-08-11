#include "Parry.h"
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>

#include "../../core/utils/InputControl.h"
#include "../../entities/character/Character.h"
#include "../../core/utils/RandomGenerator.h"
#include "../../core/utils/DialogFunctions.h"
#include "../../core/d2d-context/D2DContext.h"
#include "../../core/window/GameWindow.h"
#include "../../ui/UIManager.h"
#include "../../ui/screens/combat/ScreenCombat.h"
#include "../../rendering/direct-2d/D2DRenderer.h"
#include "../../core/utils/Color.h"

std::function<void()> Parry::onUpdateScreen = nullptr;
std::string Parry::minigameMessage = "";
std::string Parry::minigameBar = "";
int Parry::cursorPos = 0;
int Parry::sweetSpotCenter = 0;
int Parry::sweetSpotSize = 0;
int Parry::barSize = 0;
Character* Parry::inimigoAtacante = nullptr;
int Parry::parryStatus = 0;
int Parry::s_ultimoDanoRefletido = 0;

bool Parry::tentarParry(Character* atacante, Character* defensor, int danoMitigado, int& danoReduzido) 
{
    int dexterityDoAtacante = atacante ? std::max(1, atacante->getDexterity()) : 1;
    int dexterityDoDefensor = defensor ? std::max(1, defensor->getDexterity()) : 1;

    int difficulty = std::clamp(danoMitigado / 5 + (dexterityDoAtacante / 10), 1, 20);

    int sweetSpotSizeOverride = -1;
    if (defensor) {
        float ratio = (float)danoMitigado / defensor->obterVidaMaxima();
        if (ratio < 0.3f) sweetSpotSizeOverride = 6;
        else if (ratio < 0.6f) sweetSpotSizeOverride = 4;
        else sweetSpotSizeOverride = 2;
    }

    int digitOverride = -1;
    if (defensor) {
        float ratio = (float)danoMitigado / defensor->obterVidaMaxima();
        if (ratio < 0.3f) digitOverride = 4;
        else if (ratio < 0.6f) digitOverride = 6;
        else digitOverride = 8;
    }
    float typingSpeedMul = 1.0f;
    if (dexterityDoDefensor > dexterityDoAtacante) typingSpeedMul = 0.7f;
    else if (dexterityDoDefensor == dexterityDoAtacante) typingSpeedMul = 1.0f;
    else typingSpeedMul = 1.5f;

    bool sucesso = false;
    std::string nomeAtacante = atacante ? atacante->getName() : "Inimigo";

    // Notificação de aviso do Parry exigindo ENTER do jogador para iniciar
    if (Parry::onUpdateScreen) {
        Parry::minigameMessage = "PARRY! " + nomeAtacante + " VAI ATACAR! PRESSIONE ENTER PARA INICIAR (USARA ESPACO)";
        Parry::onUpdateScreen();
    }
    InputControl::limparBuffer();
    InputControl::aguardarEnter("PARRY! INIMIGO VAI ATACAR! PRESSIONE ENTER PARA INICIAR (USARA ESPACO)");
    InputControl::limparBuffer();
    Parry::minigameMessage = "";

    // Executa o minigame diretamente no Direct2D sem popups de terminal
    bool isTerminal = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isTerminal) {
        sucesso = executarMinigameDigitacao(difficulty, danoMitigado, danoReduzido, typingSpeedMul, digitOverride);
    } else {
        if (defensor && defensor->obterParryModerno()) {
            float speedMul = 1.0f;
            if (dexterityDoDefensor > dexterityDoAtacante) speedMul = 2.0f;
            else if (dexterityDoDefensor == dexterityDoAtacante) speedMul = 1.5f;
            sucesso = executarMinigameMovimento(difficulty, danoMitigado, danoReduzido, speedMul, sweetSpotSizeOverride);
        } else {
            sucesso = executarMinigameDigitacao(difficulty, danoMitigado, danoReduzido, typingSpeedMul, digitOverride);
        }
    }
    return sucesso;
}

void Parry::renderizarOverlaysD2D(D2DRenderer& d2d, int screenWidth, int screenHeight) {
    if (Parry::barSize <= 0 && Parry::minigameMessage.empty() && Parry::minigameBar.empty()) return;

    float cx = screenWidth / 2.0f;
    float cy = screenHeight / 2.0f - 40.0f;

    if (!Parry::minigameMessage.empty()) {
        std::wstring msg = std::wstring(Parry::minigameMessage.begin(), Parry::minigameMessage.end());
        d2d.desenharTexto(msg, cx - (msg.size() * 5.0f), cy, D2D1::ColorF(1.0f, 0.84f, 0.0f), 18.0f);
    }

    if (Parry::barSize > 0 && Parry::cursorPos >= 0) {
        float barW = 340.0f;
        float barH = 24.0f;
        float barLeft = cx - barW / 2.0f;
        float barTop = cy + 30.0f;

        d2d.preencherRetangulo(barLeft, barTop, barW, barH, D2D1::ColorF(0.1f, 0.1f, 0.12f, 0.85f));
        d2d.desenharRetangulo(barLeft, barTop, barW, barH, D2D1::ColorF(0.7f, 0.7f, 0.7f, 1.0f), 1.5f);

        float unitW = barW / Parry::barSize;
        float ssLeft = barLeft + (Parry::sweetSpotCenter - Parry::sweetSpotSize / 2.0f) * unitW;
        float ssW = Parry::sweetSpotSize * unitW;

        d2d.preencherRetangulo(ssLeft, barTop + 2.0f, ssW, barH - 4.0f, D2D1::ColorF(0.0f, 0.8f, 0.2f, 0.9f));
        d2d.desenharRetangulo(ssLeft, barTop + 2.0f, ssW, barH - 4.0f, D2D1::ColorF(0.2f, 1.0f, 0.4f, 1.0f), 1.5f);

        float curLeft = barLeft + Parry::cursorPos * unitW;
        d2d.preencherRetangulo(curLeft - 2.0f, barTop - 2.0f, 6.0f, barH + 4.0f, D2D1::ColorF(0.0f, 0.9f, 1.0f, 1.0f));
    } else if (!Parry::minigameBar.empty()) {
        std::wstring barText = std::wstring(Parry::minigameBar.begin(), Parry::minigameBar.end());
        d2d.desenharTexto(barText, cx - (barText.size() * 4.8f), cy + 30.0f, D2D1::ColorF(0.2f, 1.0f, 0.4f), 16.0f);
    }
}

void Parry::executarTelegraphingFlash(const std::string& nomeInimigo) {
    if (!Parry::onUpdateScreen) return;

    std::string msgOriginal = Parry::minigameMessage;
    std::string nomeStr = nomeInimigo.empty() ? "INIMIGO" : nomeInimigo;

    for (int f = 0; f < 3; ++f) {
        if (f % 2 == 0) {
            Parry::minigameMessage = "FLASH! " + nomeStr + " VAI ATACAR!";
        } else {
            Parry::minigameMessage = "PREPARE-SE PARA O PARRY!";
        }
        
        Parry::onUpdateScreen();
        if (auto* win = D2DContext::window) win->processarMensagens();
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    
    Parry::minigameMessage = msgOriginal;
}

std::string Parry::obterMensagemFeedback(bool parrySucesso, int finalDamage) {
    if (parrySucesso) {
        if (finalDamage <= 0) return "Parry Perfeito! Ataque anulado.";
        else return "Parry efetivo! -" + std::to_string(finalDamage) + " HP.";
    }
    return "Parry falhou! -" + std::to_string(finalDamage) + " HP.";
}

bool Parry::executarMinigameMovimento(int difficulty, int danoMitigado, int& danoReduzido, float speedMultiplier, int sweetSpotSizeOverride) 
{
    Parry::minigameMessage = "APERTE [ESPACO] NO ALVO!";
    
    barSize = 36;
    sweetSpotSize = (sweetSpotSizeOverride >= 0) ? sweetSpotSizeOverride : std::clamp(6 - (difficulty / 4), 2, 8);
    // Garantir que a zona verde do parry apareca apenas na metade direita da barra
    sweetSpotCenter = RandomGenerator::getInteiro(barSize / 2 + sweetSpotSize / 2, barSize - sweetSpotSize / 2 - 2);
    
    int posicaoAtual = 0;
    bool espacoPressionado = false;
    int posicaoPressionada = -1;
    
    InputControl::limparBuffer();
    
    int delayMs = std::clamp((int)(25.0f / (1.0f + (difficulty * 0.08f) * speedMultiplier)), 8, 35);
    auto minigameStart = std::chrono::steady_clock::now();

    while (posicaoAtual <= barSize) {
        cursorPos = posicaoAtual;

        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::atualizarTeclas();
        
        std::string barra = "[";
        for (int i = 0; i < barSize; i++) {
            bool noSweetSpot = (i >= sweetSpotCenter - sweetSpotSize/2 && i <= sweetSpotCenter + sweetSpotSize/2);
            if (i == cursorPos) {
                barra += "|>";
            } else if (noSweetSpot) {
                barra += "=";
            } else {
                barra += "-";
            }
        }
        barra += "]";
        Parry::minigameBar = "REACAO: " + barra;

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        }

        char tecla = InputControl::lerTecla();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - minigameStart).count();
        if (elapsedMs > 100 && tecla == ' ') {
            espacoPressionado = true;
            posicaoPressionada = posicaoAtual;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        posicaoAtual++;
    }

    cursorPos = -1;
    barSize = 0;
    Parry::minigameMessage = "";
    Parry::minigameBar = "";
    if (Parry::onUpdateScreen) {
        Parry::onUpdateScreen();
    }
    
    if (espacoPressionado) {
        bool noSweetSpot = (posicaoPressionada >= sweetSpotCenter - sweetSpotSize/2 && posicaoPressionada <= sweetSpotCenter + sweetSpotSize/2);
        if (noSweetSpot) {
            int distancia = std::abs(posicaoPressionada - sweetSpotCenter);
            if (distancia <= 1) {
                danoReduzido = danoMitigado;
            } else {
                danoReduzido = std::max(1, danoMitigado / 2);
            }
            return true;
        }
    }
    
    danoReduzido = 0;
    return false;
}

bool Parry::executarMinigameDigitacao(int difficulty, int danoMitigado, int& danoReduzido, float speedMultiplier, int digitOverride)
{
    int tamanhoSequencia = (digitOverride > 0) ? digitOverride : std::clamp(4 + difficulty / 5, 4, 8);
    std::string sequencia = "";
    for (int i = 0; i < tamanhoSequencia; ++i) {
        sequencia += std::to_string(RandomGenerator::getInteiro(0, 9));
    }

    double tempoLimite = std::max(1.5, (4.0 - difficulty / 6.0) / std::max(0.1f, speedMultiplier));

    std::string instructions = "DIGITE: " + sequencia;
    Parry::minigameMessage = instructions;

    std::string resposta = "";
    auto inicio = std::chrono::steady_clock::now();
    bool tempoEsgotado = false;
    bool concluido = false;

    InputControl::limparBuffer();

    while (true) {
        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::atualizarTeclas();

        auto agora = std::chrono::steady_clock::now();
        double decorrido = std::chrono::duration<double>(agora - inicio).count();

        if (decorrido >= tempoLimite) {
            tempoEsgotado = true;
            break;
        }

        float tempoRestante = (float)std::max(0.0, tempoLimite - decorrido);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << "DIGITADO: " << resposta;
        std::string barText = ss.str();
        for (size_t k = resposta.size(); k < (size_t)tamanhoSequencia; ++k)
            barText += "_";
        ss.str(""); ss << " [" << tempoRestante << "s]";
        Parry::minigameBar = barText;

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        }

        char c = InputControl::lerTecla();
        if (c != 0) {
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - inicio).count();
            if (c == '\r' || c == '\n') {
                if (elapsedMs > 150 && !resposta.empty()) {
                    concluido = true;
                    break;
                }
            } else if (c == '\b' || c == 127) {
                if (!resposta.empty()) {
                    resposta.pop_back();
                }
            } else if (std::isdigit(static_cast<unsigned char>(c))) {
                resposta += c;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    Parry::minigameMessage = "";
    Parry::minigameBar = "";

    if (tempoEsgotado) {
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "TEMPO ESGOTADO!";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        }
        danoReduzido = 0;
        return false;
    }

    auto fim = std::chrono::steady_clock::now();
    double tempoTotal = std::chrono::duration<double>(fim - inicio).count();

    if (concluido && resposta == sequencia) {
        if (tempoTotal <= tempoLimite * 0.5) {
            // Parry Perfeito!
            danoReduzido = danoMitigado;
        } else {
            // Parry Efetivo!
            danoReduzido = std::max(1, danoMitigado / 2);
        }
        return true;
    }

    danoReduzido = 0;
    return false;
}
