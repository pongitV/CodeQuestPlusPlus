#pragma once

#include <functional>
#include <string>

class Character;
class D2DRenderer;

class Parry 
{
public:
    static std::function<void()> onUpdateScreen;
    static std::string minigameMessage;
    static std::string minigameBar;

    // Estado da barra de movimento para renderizacao Direct2D
    static int cursorPos;
    static int sweetSpotCenter;
    static int sweetSpotSize;
    static int barSize;

    // Encapsulamento do estado do atacante e resultado de parry
    static Character* attackingEnemy;
    static Character* getAttackingEnemy() { return attackingEnemy; }
    static void setAttackingEnemy(Character* enemy) { attackingEnemy = enemy; }

    static int parryStatus;
    static int getParryStatus() { return parryStatus; }
    static void setParryStatus(int status) { parryStatus = status; }

    static int lastReflectedDamage;
    static int getLastReflectedDamage() { return lastReflectedDamage; }
    static void setLastReflectedDamage(int val) { lastReflectedDamage = val; }

    // Compatibilidade legada
    static Character*& inimigoAtacante;
    static Character* obterInimigoAtacante() { return getAttackingEnemy(); }
    static void definirInimigoAtacante(Character* enemy) { setAttackingEnemy(enemy); }
    static int obterParryStatus() { return getParryStatus(); }
    static void definirParryStatus(int status) { setParryStatus(status); }
    static int& s_ultimoDanoRefletido;
    static int obterUltimoDanoRefletido() { return getLastReflectedDamage(); }
    static void definirUltimoDanoRefletido(int val) { setLastReflectedDamage(val); }

    // Gerencia o calculo de dificuldade e aciona o minigame, retornando true se o jogador vencer
    static bool attemptParry(Character* attacker, Character* defender, int mitigatedDamage, int& reducedDamage);
    static void renderD2DOverlays(D2DRenderer& d2d, int screenWidth, int screenHeight);
    static void onAttackIncoming(Character* attacker, Character* defender);

    static std::string getFeedbackMessage(bool parrySuccess, int finalDamage);

    static bool executeMovementMinigame(int difficulty, int mitigatedDamage, int& reducedDamage, float speedMultiplier = 1.0f, int sweetSpotSizeOverride = -1);
    static bool executeTypingMinigame(int difficulty, int mitigatedDamage, int& reducedDamage, float speedMultiplier = 1.0f, int digitOverride = -1);
    static void executeTelegraphingFlash(const std::string& enemyName = "");

    // Delegados legados em portugues
    static bool tentarParry(Character* atacante, Character* defensor, int danoMitigado, int& danoReduzido) {
        return attemptParry(atacante, defensor, danoMitigado, danoReduzido);
    }
    static void renderizarOverlaysD2D(D2DRenderer& d2d, int screenWidth, int screenHeight) {
        renderD2DOverlays(d2d, screenWidth, screenHeight);
    }
    static std::string obterMensagemFeedback(bool parrySucesso, int finalDamage) {
        return getFeedbackMessage(parrySucesso, finalDamage);
    }
    static bool executarMinigameMovimento(int difficulty, int danoMitigado, int& danoReduzido, float speedMultiplier = 1.0f, int sweetSpotSizeOverride = -1) {
        return executeMovementMinigame(difficulty, danoMitigado, danoReduzido, speedMultiplier, sweetSpotSizeOverride);
    }
    static bool executarMinigameDigitacao(int difficulty, int danoMitigado, int& danoReduzido, float speedMultiplier = 1.0f, int digitOverride = -1) {
        return executeTypingMinigame(difficulty, danoMitigado, danoReduzido, speedMultiplier, digitOverride);
    }
    static void executarTelegraphingFlash(const std::string& nomeInimigo = "") {
        executeTelegraphingFlash(nomeInimigo);
    }
};
