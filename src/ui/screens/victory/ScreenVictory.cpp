#include "ScreenVictory.h"
#include <map>
#include "../../UIManager.h"
#include "../../../systems/combat/Combat.h"
#include "../../../entities/character/Character.h"
#include "../combat/ScreenCombat.h"

void TelaVitoria::display(Character* currentPlayer, int goldEarned, int xpEarned,
    int totalDamageDealt, int totalDamageTaken, int totalHealingReceived, int combatTurns,
    const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados,
    int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas)
{
    std::map<std::string, int> contagem;
    for (const std::string& item : itensObtidos) contagem[item]++;
    std::vector<std::pair<std::string, int>> dropsUnicos;
    for (auto const& [nome, qtd] : contagem) dropsUnicos.push_back({nome, qtd});

    bool podeSubirNivel = currentPlayer->podeSubirDeNivel();

    const std::string& tituloMapa = TelaCombate::contexto.tituloMapaAtual;

    GerenciadorPerspectiva::obterVitoriaUI().display(currentPlayer, goldEarned, xpEarned,
        totalDamageDealt, totalDamageTaken, totalHealingReceived, combatTurns,
        inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
}
