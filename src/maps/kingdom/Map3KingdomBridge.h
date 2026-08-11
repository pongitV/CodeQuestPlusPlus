#pragma once

#include <string>
#include <vector>

#include "../../entities/character/Character.h"
#include "../interfaces/IMap.h"

// KingdomBridgeMap represents the Bridge to Kingdom transition map zone.
// Controls bridge tile matrix, Troll boss trigger, and zone transition logic.
class Mapa3PonteReino final : public IMap 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Character* currentPlayer;
    
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;
    NextMapTransition proximoMapa;

public:
    explicit Mapa3PonteReino(Character* personagemJogador);
    ~Mapa3PonteReino() override;

    std::string getTitle() const override { return tituloDoMapaAtual; }
    std::string obterTitulo() const override { return getTitle(); }

    NextMapTransition iniciarLoopDeExploracao() override;
    NextMapTransition startExplorationLoop() override { return iniciarLoopDeExploracao(); }
};

using KingdomBridgeMap = Mapa3PonteReino;
