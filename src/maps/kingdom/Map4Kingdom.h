#pragma once

#include <string>
#include <vector>

#include "../../entities/character/Character.h"
#include "../interfaces/IMap.h"

// KingdomMap represents the capital Kingdom map zone.
// Controls main kingdom zone, Church sub-map, final boss encounters, and story conclusions.
class Mapa4Reino final : public IMap 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Character* currentPlayer;
    
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;
    NextMapTransition proximoMapa;

    // Church sub-map state control
    bool jogadorEstaDentroDeUmSubMapa;
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    std::vector<std::string> matrizDoMapaDaIgrejaSalva;
    bool igrejaJaFoiVisitada;

public:
    explicit Mapa4Reino(Character* personagemJogador);
    ~Mapa4Reino() override;

    std::string getTitle() const override { return tituloDoMapaAtual; }
    std::string obterTitulo() const override { return getTitle(); }

    NextMapTransition iniciarLoopDeExploracao() override;
    NextMapTransition startExplorationLoop() override { return iniciarLoopDeExploracao(); }
};

using KingdomMap = Mapa4Reino;
