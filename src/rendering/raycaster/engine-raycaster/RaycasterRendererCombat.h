#pragma once

#include <vector>
#include <string>
#include <tuple>
#include "../../../entities/character/Character.h"
#include "RaycasterSprites.h"

class RaycasterRendererCombate {
public:
    // Renderiza o fundo 3D estatico e sobrepoe a arte do inimigo
    static std::vector<std::string> renderizarQuadro(
        const std::string& tituloMapa, 
        Character* jogador, 
        const std::vector<Character*>& enemies,
        Character* alvoAnimacao = nullptr,
        int frame = 0,
        int framesDeDanoJogador = 0,
        int danoAmount = -1,
        bool isCura = false,
        int tempoMs = 0,
        bool isMorte = false,
        const std::vector<std::string>& dropsAnimacao = {},
        float spriteOpacity = 1.0f
    );

    static void renderizarQuadroD2D(
        std::vector<Pixel3D>& screen,
        int LARGURA_TELA,
        int ALTURA_TELA,
        Character* jogador, 
        const std::vector<Character*>& enemies,
        Character* alvoAnimacao = nullptr,
        int frame = 0,
        int framesDeDanoJogador = 0,
        int danoAmount = -1,
        bool isCura = false,
        int tempoMs = 0,
        bool isMorte = false,
        const std::vector<std::string>& dropsAnimacao = {},
        float spriteOpacity = 1.0f
    );

    // Gera uma mini-arena baseada no titulo do map (bioma)
    static std::vector<std::string> obterArenaPorTitulo(const std::string& titulo);

    static const std::vector<std::string>& obterUltimoFundoRenderizado();

    static std::tuple<int,int,int> obterCorSpriteInimigo(Character* enemy);

    // Pinta uma string de texto sobre o buffer 1D da tela 3D (sobreposicao)
    static void pintarTextoNoBuffer(std::vector<std::string>& screen, int larguraTela, int alturaMax, int posX, int posY, const std::string& texto, const std::string& corFg, const std::string& corBgOverride = "");

private:
    // Pega as linhas de arte ASCII do monstro e pinta por cima das strings renderizadas do motor 3D
    static void sobreporSprite(
        std::vector<std::string>& screen, 
        Character* enemy, 
        int inimigoIdx,
        int totalInimigos,
        int larguraTela, 
        int alturaTela, 
        int flashDanoInimigo, 
        int danoAmount, 
        bool isCura, 
        int tempoMs, 
        bool isMorte = false, 
        int frameMorte = 0, 
        const std::vector<std::string>& dropsAnimacao = {}, 
        bool isSelecionado = false,
        float spriteOpacity = 1.0f
    );

    static void sobreporSpriteD2D(
        std::vector<Pixel3D>& screen, 
        Character* enemy, 
        int inimigoIdx,
        int totalInimigos,
        int larguraTela, 
        int alturaTela, 
        int flashDanoInimigo, 
        int danoAmount, 
        bool isCura, 
        int tempoMs, 
        bool isMorte = false, 
        int frameMorte = 0, 
        const std::vector<std::string>& dropsAnimacao = {}, 
        bool isSelecionado = false,
        float spriteOpacity = 1.0f
    );
};
