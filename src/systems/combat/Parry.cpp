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
Character* Parry::attackingEnemy = nullptr;
Character*& Parry::inimigoAtacante = Parry::attackingEnemy;
int Parry::parryStatus = 0;
int Parry::lastReflectedDamage = 0;
int& Parry::s_ultimoDanoRefletido = Parry::lastReflectedDamage;

bool Parry::attemptParry(Character* attacker, Character* defender, int mitigatedDamage, int& reducedDamage) 
{
    int attackerDexterity = attacker ? std::max(1, attacker->getDexterity()) : 1;
    int defenderDexterity = defender ? std::max(1, defender->getDexterity()) : 1;

    int difficulty = std::clamp(mitigatedDamage / 5 + (attackerDexterity / 10), 1, 20);

    int sweetSpotSizeOverride = -1;
    if (defender) {
        float ratio = (float)mitigatedDamage / defender->getMaxHealth();
        if (ratio < 0.3f) sweetSpotSizeOverride = 6;
        else if (ratio < 0.6f) sweetSpotSizeOverride = 4;
        else sweetSpotSizeOverride = 2;
    }

    int digitOverride = -1;
    if (defender) {
        float ratio = (float)mitigatedDamage / defender->getMaxHealth();
        if (ratio < 0.3f) digitOverride = 4;
        else if (ratio < 0.6f) digitOverride = 6;
        else digitOverride = 8;
    }
    float typingSpeedMul = 1.0f;
    if (defenderDexterity > attackerDexterity) typingSpeedMul = 0.7f;
    else if (defenderDexterity == attackerDexterity) typingSpeedMul = 1.0f;
    else typingSpeedMul = 1.5f;

    bool success = false;
    std::string attackerName = attacker ? attacker->getName() : "Inimigo";

    // Notificacao de aviso do Parry exigindo ENTER do jogador para iniciar
    if (Parry::onUpdateScreen) {
        Parry::minigameMessage = "PARRY! " + attackerName + " VAI ATACAR! PRESSIONE ENTER PARA INICIAR (USARA ESPACO)";
        Parry::onUpdateScreen();
    }
    InputControl::clearBuffer();
    InputControl::waitForEnter("PARRY! INIMIGO VAI ATACAR! PRESSIONE ENTER PARA INICIAR (USARA ESPACO)");
    InputControl::clearBuffer();
    Parry::minigameMessage = "";

    // Executa o minigame diretamente no Direct2D sem popups de terminal
    bool isTerminal = !GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
    if (isTerminal) {
        success = executeTypingMinigame(difficulty, mitigatedDamage, reducedDamage, typingSpeedMul, digitOverride);
    } else {
        if (defender && defender->isModernParry()) {
            float speedMul = 1.0f;
            if (defenderDexterity > attackerDexterity) speedMul = 2.0f;
            else if (defenderDexterity == attackerDexterity) speedMul = 1.5f;
            success = executeMovementMinigame(difficulty, mitigatedDamage, reducedDamage, speedMul, sweetSpotSizeOverride);
        } else {
            success = executeTypingMinigame(difficulty, mitigatedDamage, reducedDamage, typingSpeedMul, digitOverride);
        }
    }
    return success;
}

void Parry::renderD2DOverlays(D2DRenderer& d2d, int screenWidth, int screenHeight) {
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

void Parry::executeTelegraphingFlash(const std::string& enemyName) {
    if (!Parry::onUpdateScreen) return;

    std::string originalMsg = Parry::minigameMessage;
    std::string enemyStr = enemyName.empty() ? "INIMIGO" : enemyName;

    for (int f = 0; f < 3; ++f) {
        if (f % 2 == 0) {
            Parry::minigameMessage = "FLASH! " + enemyStr + " VAI ATACAR!";
        } else {
            Parry::minigameMessage = "PREPARE-SE PARA O PARRY!";
        }
        
        Parry::onUpdateScreen();
        if (auto* win = D2DContext::window) win->processarMensagens();
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    
    Parry::minigameMessage = originalMsg;
}

std::string Parry::getFeedbackMessage(bool parrySuccess, int finalDamage) {
    if (parrySuccess) {
        if (finalDamage <= 0) return "Parry Perfeito! Ataque anulado.";
        else return "Parry efetivo! -" + std::to_string(finalDamage) + " HP.";
    }
    return "Parry falhou! -" + std::to_string(finalDamage) + " HP.";
}

bool Parry::executeMovementMinigame(int difficulty, int mitigatedDamage, int& reducedDamage, float speedMultiplier, int sweetSpotSizeOverride) 
{
    Parry::minigameMessage = "APERTE [ESPACO] NO ALVO!";
    
    barSize = 36;
    sweetSpotSize = (sweetSpotSizeOverride >= 0) ? sweetSpotSizeOverride : std::clamp(6 - (difficulty / 4), 2, 8);
    // Garantir que a zona verde do parry apareca apenas na metade direita da barra
    sweetSpotCenter = RandomGenerator::getInt(barSize / 2 + sweetSpotSize / 2, barSize - sweetSpotSize / 2 - 2);
    
    int currentPosition = 0;
    bool spacePressed = false;
    int pressedPosition = -1;
    
    InputControl::clearBuffer();
    
    int delayMs = std::clamp((int)(25.0f / (1.0f + (difficulty * 0.08f) * speedMultiplier)), 8, 35);
    auto minigameStart = std::chrono::steady_clock::now();

    while (currentPosition <= barSize) {
        cursorPos = currentPosition;

        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::updateKeys();
        
        std::string bar = "[";
        for (int i = 0; i < barSize; i++) {
            bool inSweetSpot = (i >= sweetSpotCenter - sweetSpotSize/2 && i <= sweetSpotCenter + sweetSpotSize/2);
            if (i == cursorPos) {
                bar += "|>";
            } else if (inSweetSpot) {
                bar += "=";
            } else {
                bar += "-";
            }
        }
        bar += "]";
        Parry::minigameBar = "REACAO: " + bar;

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        }

        char key = InputControl::readKey();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - minigameStart).count();
        if (elapsedMs > 100 && key == ' ') {
            spacePressed = true;
            pressedPosition = currentPosition;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        currentPosition++;
    }

    cursorPos = -1;
    barSize = 0;
    Parry::minigameMessage = "";
    Parry::minigameBar = "";
    if (Parry::onUpdateScreen) {
        Parry::onUpdateScreen();
    }
    
    if (spacePressed) {
        bool inSweetSpot = (pressedPosition >= sweetSpotCenter - sweetSpotSize/2 && pressedPosition <= sweetSpotCenter + sweetSpotSize/2);
        if (inSweetSpot) {
            int distance = std::abs(pressedPosition - sweetSpotCenter);
            if (distance <= 1) {
                reducedDamage = mitigatedDamage;
            } else {
                reducedDamage = std::max(1, mitigatedDamage / 2);
            }
            return true;
        }
    }
    
    reducedDamage = 0;
    return false;
}

bool Parry::executeTypingMinigame(int difficulty, int mitigatedDamage, int& reducedDamage, float speedMultiplier, int digitOverride)
{
    int sequenceSize = (digitOverride > 0) ? digitOverride : std::clamp(4 + difficulty / 5, 4, 8);
    std::string sequence = "";
    for (int i = 0; i < sequenceSize; ++i) {
        sequence += std::to_string(RandomGenerator::getInt(0, 9));
    }

    double timeLimit = std::max(1.5, (4.0 - difficulty / 6.0) / std::max(0.1f, speedMultiplier));

    std::string instructions = "DIGITE: " + sequence;
    Parry::minigameMessage = instructions;

    std::string answer = "";
    auto start = std::chrono::steady_clock::now();
    bool timeExpired = false;
    bool completed = false;

    InputControl::clearBuffer();

    while (true) {
        if (auto* win = D2DContext::window) win->processarMensagens();
        InputControl::updateKeys();

        auto now = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(now - start).count();

        if (elapsed >= timeLimit) {
            timeExpired = true;
            break;
        }

        float remainingTime = (float)std::max(0.0, timeLimit - elapsed);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << "DIGITADO: " << answer;
        std::string barText = ss.str();
        for (size_t k = answer.size(); k < (size_t)sequenceSize; ++k)
            barText += "_";
        ss.str(""); ss << " [" << remainingTime << "s]";
        barText += ss.str();
        Parry::minigameBar = barText;

        if (Parry::onUpdateScreen) {
            Parry::onUpdateScreen();
        }

        char c = InputControl::readKey();
        if (c != 0) {
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
            if (c == '\r' || c == '\n') {
                if (elapsedMs > 150 && !answer.empty()) {
                    completed = true;
                    break;
                }
            } else if (c == '\b' || c == 127) {
                if (!answer.empty()) {
                    answer.pop_back();
                }
            } else if (std::isdigit(static_cast<unsigned char>(c))) {
                answer += c;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    Parry::minigameMessage = "";
    Parry::minigameBar = "";

    if (timeExpired) {
        if (Parry::onUpdateScreen) {
            Parry::minigameMessage = "TEMPO ESGOTADO!";
            Parry::onUpdateScreen();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Parry::minigameMessage = "";
            Parry::onUpdateScreen();
        }
        reducedDamage = 0;
        return false;
    }

    auto end = std::chrono::steady_clock::now();
    double totalTime = std::chrono::duration<double>(end - start).count();

    if (completed && answer == sequence) {
        if (totalTime <= timeLimit * 0.5) {
            // Parry Perfeito!
            reducedDamage = mitigatedDamage;
        } else {
            // Parry Efetivo!
            reducedDamage = std::max(1, mitigatedDamage / 2);
        }
        return true;
    }

    reducedDamage = 0;
    return false;
}
