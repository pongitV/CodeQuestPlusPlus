#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../../entities/character/Character.h"
#include "../interfaces/IMap.h"

// Representa a zona da floresta no mapa do mundo.
// Handles sub-maps (Tree Heart, Maze, Boss Room), tile matrix, and environmental interactions.
class Mapa2Floresta final : public IMap 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Character* currentPlayer;
    
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;
 
    std::vector<std::string> matrizDoMapaDoCoracaoDaArvoreSalva;
    std::vector<std::string> matrizDoMapaDoLabirintoSalva;
    std::vector<std::string> matrizDoMapaSalaDoChefeSalva;
    bool coracaoDaArvoreJaFoiVisitado;
    bool labirintoJaFoiVisitado;
    bool salaDoChefeJaFoiVisitada;
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;
    NextMapTransition proximoMapa;

    std::unordered_map<char, std::unique_ptr<ForestInteraction>> interacoes;

public:
    explicit Mapa2Floresta(Character* personagemJogador);
    ~Mapa2Floresta() override;
    
    std::string getTitle() const override { return tituloDoMapaAtual; }
    std::string obterTitulo() const override { return getTitle(); }

    NextMapTransition iniciarLoopDeExploracao() override;
    NextMapTransition startExplorationLoop() override { return iniciarLoopDeExploracao(); }

private:
    void initializeInteracoes();
};

using ForestMap = Mapa2Floresta;
