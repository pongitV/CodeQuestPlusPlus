#pragma once

#include <vector>
#include <string>
#include "../../../entities/character/Character.h"

struct CombatContexte {
    bool isModo3D = false;
    bool isTerminalView = false;
    std::vector<std::string> matrizDoMapaAtual;
    float jogadorPosX = 0;
    float jogadorPosY = 0;
    float jogadorAngulo = 0;
    std::string tituloMapaAtual;

    int turnoAtualVisivel = 0;
    std::string nomeTurnoVisivel;
    int selecaoAcaoAtual = 0;
    int selecaoAlvoAtual = -1;
    bool piscarSelecao = false;

    Character* inimigoMortoComDrops = nullptr;
    std::vector<std::string> dropsAtivos;
    std::vector<std::string> opcoesMenuAtual;
    Character* personagemHUD = nullptr;

    void configurar(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
        isModo3D = modo3D;
        matrizDoMapaAtual = matriz;
        jogadorPosX = posX;
        jogadorPosY = posY;
        jogadorAngulo = angulo;
        tituloMapaAtual = titulo;
    }

    void definirTurnoVisivel(int turno, const std::string& nome) {
        turnoAtualVisivel = turno;
        nomeTurnoVisivel = nome;
    }
};
