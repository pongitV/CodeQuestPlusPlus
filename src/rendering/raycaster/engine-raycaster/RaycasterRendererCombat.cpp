#include "RaycasterRendererCombat.h"
#include "Raycaster.h"
#include "RaycasterSprites.h"
#include "../../../entities/races/RaceBase.h"
#include "../../../ui/screens/combat/ScreenCombat.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include "../../../core/utils/Color.h"

#include "../../../systems/combat/Parry.h"

// Arena de combate por bioma
std::vector<std::string> RaycasterRendererCombate::obterArenaPorTitulo(const std::string& titulo) {
    std::string upper = titulo;
    for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

    char chao = '.';
    if (upper.find("VILA") != std::string::npos || upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos || upper.find("INICIO") != std::string::npos) {
        chao = ',';
    }

    auto chaoStr = [&](int n) { return std::string(n, chao); };

    if (upper.find("CHEFE") != std::string::npos) {
        return {
            "  .........................  ",
            " .............................  ",
            "...............................",
            "...............................",
            "...............................",
            "...............................",
            "...............................",
            " ............................. ",
            "  ...........................  "
        };
    }
    if (upper.find("SALA DE TROFEUS") != std::string::npos || upper.find("TROFEU") != std::string::npos) {
        return {
            "#############################",
            "#.......T...T...T...T.......#",
            "#...........................#",
            "#.......*...*...*...*.......#",
            "#...........................#",
            "#############################"
        };
    }
    if (upper.find("LABIRINTO") != std::string::npos) {
        return {
            "=================================",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "|...............................|",
            "================================="
        };
    }
    if (upper.find("CAVERNA") != std::string::npos || upper.find("CORACAO") != std::string::npos) {
        return {
            "################################",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "################################"
        };
    }
    if (upper.find("PATIO DO REINO") != std::string::npos || upper.find("REINO") != std::string::npos) {
        return {
            "#|||||||||||||||||||||||||||||||#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#|||||||||||||||||||||||||||||||#"
        };
    }
    if (upper.find("CEMITERIO") != std::string::npos) {
        return {
            "################################",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "################################"
        };
    }
    if (upper.find("FLORESTA") != std::string::npos || upper.find("BOSQUE") != std::string::npos) {
        return {
            "################################",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "#" + chaoStr(30) + "#",
            "################################"
        };
    }
    // Village / Inicio
    if (upper.find("VILA") != std::string::npos || upper.find("INICIO") != std::string::npos) {
        return {
            "T=====[]=======================T",
            "T" + chaoStr(30) + "T",
            "T" + chaoStr(30) + "T",
            "T" + chaoStr(30) + "T",
            "T" + chaoStr(30) + "T",
            "T" + chaoStr(30) + "T",
            "T" + chaoStr(30) + "T",
            "T" + chaoStr(30) + "T",
            "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"
        };
    }
    
    // Default
    return {
        "####_[]_########################",
        "#" + chaoStr(30) + "#",
        "#" + chaoStr(30) + "#",
        "#" + chaoStr(30) + "#",
        "#" + chaoStr(30) + "#",
        "#" + chaoStr(30) + "#",
        "#" + chaoStr(30) + "#",
        "#" + chaoStr(30) + "#",
        "################################"
    };
}

// Cor base do sprite do inimigo (mesmas cores do RaycasterInimigos)
std::tuple<int,int,int> RaycasterRendererCombate::obterCorSpriteInimigo(Character* enemy) {
    if (!enemy) return {255, 255, 255};
    
    switch (enemy->obterRaceType()) {
        case RaceType::Goblin: return {100, 200, 50};
        case RaceType::Ork:
        case RaceType::OrkExilado: return {50, 150, 50};
        case RaceType::Slime: return {50, 200, 255};
        case RaceType::Fairy: return {255, 100, 200};
        case RaceType::AbominacaoFloresta: return {139, 69, 19};
        case RaceType::Troll: return {150, 150, 160};
        case RaceType::Mimic: return {200, 150, 50};
        case RaceType::Mahoraga: return {255, 255, 255};
        default: return {200, 200, 200}; // Default cinza claro
    }
}

// Pintar texto no buffer 1D (overlay)
void RaycasterRendererCombate::pintarTextoNoBuffer(std::vector<std::string>& screen, int larguraTela, int alturaMax, int posX, int posY, const std::string& texto, const std::string& corFg, const std::string& corBgOverride) {
    (void)larguraTela;
    (void)corFg;
    (void)corBgOverride;
    if (posY < 0 || posY >= (int)screen.size() || posY >= alturaMax) return;
    screen[posY] = texto;
}

// Renderizar quadro principal
static std::vector<std::string> s_cachedBackground;
static std::string s_cachedTituloMapa;
static int s_cachedLarguraTela = 0;
static int s_cachedAltura3D = 0;

const std::vector<std::string>& RaycasterRendererCombate::obterUltimoFundoRenderizado() {
    return s_cachedBackground;
}

std::vector<std::string> RaycasterRendererCombate::renderizarQuadro(
    const std::string& tituloMapa, 
    Character* jogador, 
    const std::vector<Character*>& enemies,
    Character* alvoAnimacao,
    int frame,
    int framesDeDanoJogador,
    int danoAmount,
    bool isCura,
    int tempoMs,
    bool isMorte,
    const std::vector<std::string>& dropsAnimacao,
    float spriteOpacity
) {
    // Gera uma arena dedicada ao bioma
    std::vector<std::string> arena = obterArenaPorTitulo(tituloMapa);
    
    // Posicao fixa: centro da arena, olhando para Norte
    float jX = static_cast<float>(arena[0].size()) / 2.0f;
    float jY = static_cast<float>(arena.size()) - 2.0f;
    float angulo = -1.57f; // Olhando pro Norte

    int larguraTela = 120;
    int alturaTerminal = 40;
    if (larguraTela <= 0) larguraTela = 120;
    if (alturaTerminal <= 0) alturaTerminal = 40;

    int alturaHUD = 0;
    int altura3D = std::max(10, alturaTerminal - alturaHUD);

    if (s_cachedBackground.empty() || s_cachedTituloMapa != tituloMapa || s_cachedLarguraTela != larguraTela || s_cachedAltura3D != altura3D) {
        s_cachedBackground = Raycaster::desenharQuadroEstatico3D(arena, jX, jY, angulo, tituloMapa, jogador, altura3D);
        s_cachedTituloMapa = tituloMapa;
        s_cachedLarguraTela = larguraTela;
        s_cachedAltura3D = altura3D;
    }

    std::vector<std::string> screen = s_cachedBackground;

    // Sobrepoe os enemies
    int numInimigos = static_cast<int>(enemies.size());
    for (int i = 0; i < numInimigos; ++i) {
        Character* enemy = enemies[i];
        if (enemy && (enemy->obterVida() > 0 || !enemy->obterMorteAnimada())) {
            bool isAnimado = (alvoAnimacao != nullptr && enemy == alvoAnimacao);
            int framesDano = (isAnimado && danoAmount > 0 && !isMorte) ? frame : 0;
            bool isMorteIni = (isMorte && isAnimado);
            int frameMorteIni = isMorteIni ? frame : 0;
            bool isSel = (TelaCombate::contexto.selecaoAlvoAtual == i);
            
            sobreporSprite(screen, enemy, i, numInimigos, larguraTela, altura3D, framesDano, danoAmount, isCura, tempoMs, isMorteIni, frameMorteIni, dropsAnimacao, isSel, spriteOpacity);
        }
    }

    // PREENCHENDO ATE A ALTURA_TELA TOTAL PARA EVITAR CRASH NO HUD!
    std::vector<std::string> linhasRenderizadas(alturaTerminal);
    
    int cameraOffsetX = 0;
    if (framesDeDanoJogador > 0 && framesDeDanoJogador % 2 == 0) {
        cameraOffsetX = 4;
    }
    
    for (int y = 0; y < altura3D; y++) {
        std::string linha = "";
        for (int x = 0; x < larguraTela; x++) {
            int srcX = x - cameraOffsetX;
            if (srcX >= 0 && srcX < larguraTela) {
                int idx = y * larguraTela + srcX;
                if (idx < 0 || idx >= (int)screen.size()) {
                    linha += "X"; // Erro
                } else {
                    linha += screen[idx];
                }
            } else {
                linha += " "; // pixel vazio
            }
        }
        linhasRenderizadas[y] = std::move(linha);
    }
    // Preenche o restante com espacos vazios para evitar ultrapassar os limites no rodape do HUD
    for (int y = altura3D; y < alturaTerminal; y++) {
        linhasRenderizadas[y] = std::string(larguraTela, ' ');
    }

    return linhasRenderizadas;
}

// Sobrepor sprite do inimigo (com arte 3D texturizada)
void RaycasterRendererCombate::sobreporSprite(
    std::vector<std::string>& screen, 
    Character* enemy, 
    int inimigoIdx,
    int totalInimigos,
    int larguraTela, 
    int alturaVisivel, 
    int flashDanoInimigo, 
    int danoAmount, 
    bool isCura, 
    int tempoMs, 
    bool isMorte, 
    int frameMorte, 
    const std::vector<std::string>& dropsAnimacao, 
    bool isSelecionado,
    float spriteOpacity
) {
    // Usa a arte de mapa (mesma do raycaster) em vez da arte de combate 2D
    const std::vector<std::string>& arteOriginalInimigo = enemy->obterRaca()->getRaceAppearance();
    if (arteOriginalInimigo.empty()) return;

    (void)dropsAnimacao;

    double progress = 0.0;
    int totalFramesMorte = 0;
    if (isMorte) {
        totalFramesMorte = 12; // 12 frames em 3D
        progress = std::min(1.0, static_cast<double>(frameMorte) / totalFramesMorte);
    }

    // Obtem cor base para texturizacao estilo raycaster
    auto [baseR, baseG, baseB] = obterCorSpriteInimigo(enemy);

    std::vector<std::string> arteUsada = arteOriginalInimigo;
    
    int alturaArte = static_cast<int>(arteUsada.size());
    float fator = static_cast<float>(1);
    
    RaceType tipo = enemy->obterRaceType();
    bool isBoss = enemy->isBoss();
    
    switch (tipo) {
        case RaceType::Ork:
        case RaceType::OrkExilado:
            fator = 2.7f;
            break;
        case RaceType::Goblin:
            fator = 2.5f;
            break;
        case RaceType::Slime:
            fator = 2.5f;
            break;
        case RaceType::Mahoraga:
            fator = 3.0f;
            break;
        case RaceType::AbominacaoFloresta:
            fator = 1.5f;
            break;
        case RaceType::Troll:
            fator = 1.9f;
            break;
        case RaceType::Mimic:
            fator = 2.5f;
            break;
        case RaceType::Fairy:
            fator = 3.2f;
            break;
        default:
            break;
    }
    
    if (!isBoss) {
        // Variacao de escala deterministica em ate 5%
        size_t h = reinterpret_cast<size_t>(enemy);
        float pct = ((h % 101) - 50.0f) / 1000.0f; // Varia de -0.05 a +0.05
        fator *= (1.0f + pct);
    }
    
    if (alturaArte > 10) {
        arteUsada.clear();
        alturaArte = static_cast<int>(arteUsada.size());
    }
    
    int reservedBottom = 14;

    int larguraArte = 0;
    for (const auto& linha : arteUsada) {
        int comp = (int)(linha).length();
        if (comp > larguraArte) larguraArte = comp;
    }

    // Balanco horizontal baseado no tempo (apenas se nao estiver no meio da animacao de morte)
    int swayOff = 0;
    if (!isMorte) {
        int stepSway = (tempoMs / 200) % 8;
        if (stepSway < 0) stepSway += 8;
        int swayPattern[] = {0, 1, 2, 1, 0, -1, -2, -1};
        swayOff = swayPattern[stepSway] * 2;
    }
    
    int maxStartY = alturaVisivel - alturaArte - reservedBottom;
    int startY = (alturaVisivel - alturaArte) / 2;
    startY += alturaVisivel / 10;
    if (startY > maxStartY) startY = maxStartY;
    if (startY < 0) startY = 0;
    
    int startX = 0; // Sera recalculado apos obter o croppedWidth

    // Helper para extrair o background da celula do Raycaster
    auto getBg = [](const std::string& s) {
        (void)s;
        return std::string("");
    };

    auto parseAnsiRGB = [](const std::string& str) -> std::tuple<int,int,int> {
        (void)str;
        return {0, 0, 0};
    };

    // Texturiza cada caractere da arte com cores baseadas no caractere
    auto obterTgtRGB = [&](char c, int /*rx*/, int ry) -> std::tuple<int,int,int> {
        int currentBaseR = baseR;
        int currentBaseG = baseG;
        int currentBaseB = baseB;
        if (enemy->obterRaceType() == RaceType::Mahoraga && (ry * fator) < 24) {
            currentBaseR = 255;
            currentBaseG = 215;
            currentBaseB = 0; // Amarelo/Dourado
        }
        int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
        if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = currentBaseR * 0.4; gMod = currentBaseG * 0.4; bMod = currentBaseB * 0.4; }
        else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = currentBaseR * 0.6; gMod = currentBaseG * 0.6; bMod = currentBaseB * 0.6; }
        else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = currentBaseR * 0.8; gMod = currentBaseG * 0.8; bMod = currentBaseB * 0.8; }
        else if (c == '-' || c == '~' || c == ':' || c == ';') { rMod = std::min(255, (int)(currentBaseR * 1.2)); gMod = std::min(255, (int)(currentBaseG * 1.2)); bMod = std::min(255, (int)(currentBaseB * 1.2)); }
        else if (c == '.' || c == ',' || c == '\'') { rMod = std::min(255, (int)(currentBaseR * 1.5)); gMod = std::min(255, (int)(currentBaseG * 1.5)); bMod = std::min(255, (int)(currentBaseB * 1.5)); }
        else if (c == '_' || c == '|' || c == '\\' || c == '/' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '<' || c == '>') { rMod = currentBaseR * 0.5; gMod = currentBaseG * 0.5; bMod = currentBaseB * 0.5; }
        
        if (flashDanoInimigo > 0 && flashDanoInimigo % 2 == 0) {
            if (isCura) {
                rMod = 50; gMod = 255; bMod = 50;
            } else {
                rMod = 255; gMod = 50; bMod = 50;
            }
        } else if (enemy == Parry::obterInimigoAtacante()) {
            rMod = 255; gMod = 140; bMod = 0;
        } else if (isSelecionado) {
            if (TelaCombate::contexto.piscarSelecao) {
                rMod = (rMod + 255) / 2;
                gMod = (gMod + 255) / 2;
                bMod = bMod / 2;
            } else {
                int cinza = (rMod + gMod + bMod) / 3;
                rMod = (rMod + cinza) / 2;
                gMod = (gMod + cinza) / 2;
                bMod = (bMod + cinza) / 2;
            }
        }

        if (isMorte) {
            double fade = 1.0 - progress;
            rMod = static_cast<int>(rMod * fade);
            gMod = static_cast<int>(gMod * fade);
            bMod = static_cast<int>(bMod * fade);
        }
        
        return {rMod, gMod, bMod};
    };

    // Auto-crop horizontal
    int minX = larguraArte, maxX = 0;
    for (const auto& linha : arteUsada) {
        std::string limpo = linha;
        for (int i = 0; i < static_cast<int>(limpo.length()); ++i) {
            if (limpo[i] != ' ') {
                if (i < minX) minX = i;
                if (i > maxX) maxX = i;
            }
        }
    }
    if (minX > maxX) { minX = 0; maxX = larguraArte - 1; }
    int croppedWidth = maxX - minX + 1;
    
    int larguraColuna = larguraTela / totalInimigos;
    int centroColunaX = inimigoIdx * larguraColuna + larguraColuna / 2;
    startX = centroColunaX - croppedWidth / 2 + swayOff;

    bool desenharCorpo = (enemy->obterVida() > 0 || !enemy->obterMorteAnimada());

    if (desenharCorpo) {
        // Desenha contorno preto (borda do sprite) e corpo texturizado (mesclando fundo via spriteOpacity)
        for (int y = 0; y < alturaArte; y++) {
            int telaY = startY + y;
            if (telaY >= 0 && telaY < alturaVisivel) {
                std::string linhaSemColor = arteUsada[y];
                
                for (int rawX = minX; rawX <= maxX; rawX++) {
                    int x = rawX - minX;
                    int telaX = startX + x;
                    if (telaX >= 0 && telaX < larguraTela && rawX < static_cast<int>(linhaSemColor.length())) {
                        char c = linhaSemColor[rawX];
                        
                        if (c != ' ') {
                            if (isMorte) {
                                // Efeito de desintegracao dithered (virando poeira)
                                int hash = (rawX * 37 + y * 57) % 100;
                                if (hash < progress * 100) {
                                    // Renderiza particulas de poeira '.' flutuantes ou some o pixel
                                    if (progress < 0.8 && (hash % 3 == 0)) {
                                        screen[telaY * larguraTela + telaX] = ".";
                                    }
                                    continue;
                                }
                            }

                            int idx = telaY * larguraTela + telaX;
                            if (idx >= 0 && idx < (int)screen.size()) {
                                screen[idx] = " ";
                            }
                        }
                    }
                }
            }
        }
    }

    // Helper local para dividir string UTF-8
    auto splitUTF8 = [](const std::string& str) -> std::vector<std::string> {
        std::vector<std::string> chars;
        for (size_t i = 0; i < str.length(); ) {
            unsigned char c = (unsigned char)str[i];
            int len = 1;
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            chars.push_back(str.substr(i, len));
            i += len;
        }
        return chars;
    };

    // Helper para desenhar strings no buffer (com suporte a UTF-8)
    auto paintStr = [&](int posX, int posY, const std::string& txt, const std::string& color, const std::string& forcedBg = "") {
        (void)color;
        (void)forcedBg;
        if (posY < 0 || posY >= alturaVisivel) return;
        std::vector<std::string> chars = splitUTF8(txt);
        int len = static_cast<int>(chars.size());
        int drawX = posX - len/2;
        for (int i = 0; i < len; ++i) {
            int tx = drawX + i;
            if (tx >= 0 && tx < larguraTela) {
                screen[posY * larguraTela + tx] = chars[i];
            }
        }
    };

    // Helper para desenhar strings no buffer (com suporte a UTF-8, alinhado a esquerda)
    auto paintStrLeft = [&](int posX, int posY, const std::string& txt, const std::string& color, const std::string& forcedBg = "") {
        (void)color;
        (void)forcedBg;
        if (posY < 0 || posY >= alturaVisivel) return;
        std::vector<std::string> chars = splitUTF8(txt);
        int len = static_cast<int>(chars.size());
        for (int i = 0; i < len; ++i) {
            int tx = posX + i;
            if (tx >= 0 && tx < larguraTela) {
                screen[posY * larguraTela + tx] = chars[i];
            }
        }
    };

    // So desenha nameplate/HP bar se nao estiver morrendo e se spriteOpacity >= 1.0f
    if (desenharCorpo && !isMorte && spriteOpacity >= 1.0f) {
        int nameY = startY - 2;
        if (nameY >= 0) {
            std::string nameplate = enemy->getName();
            if (totalInimigos > 1) {
                nameplate += " (" + std::to_string(inimigoIdx + 1) + ")";
            }
            std::string nameColor = "";
            
            if (isSelecionado) {
                nameplate = "> " + nameplate;
            }
            
            paintStr(startX + croppedWidth/2, nameY, nameplate, nameColor);
            
            int hpY = startY - 1;
            if (hpY >= 0) {
                double pct = static_cast<double>(enemy->obterVida()) / std::max(1, enemy->obterVidaMaxima());
                std::string hpValStr = std::to_string(enemy->obterVida()) + "/" + std::to_string(enemy->obterVidaMaxima());
                
                int totalLen = 5 + 8 + 2 + (int)hpValStr.length(); // "HP: [" (5) + 8 blocks + "] " (2) + hpValStr
                int drawX = (startX + croppedWidth / 2) - totalLen / 2;
                
                // 1. "HP: ["
                paintStrLeft(drawX, hpY, "HP: [", "");
                drawX += 5;
                
                // 2. Blocos com gradiente
                int blocks = 8;
                int qtdReal = static_cast<int>(pct * blocks * 8);
                
                for (int i = 0; i < blocks; ++i) {
                    std::string corAtual = "";
                    int charIdx = i * 8;
                    if (qtdReal >= charIdx + 4) {
                        paintStrLeft(drawX + i, hpY, "█", corAtual);
                    } else {
                        paintStrLeft(drawX + i, hpY, "░", "");
                    }
                }
                drawX += blocks;
                
                // 3. "] "
                paintStrLeft(drawX, hpY, "] ", "");
                drawX += 2;
                
                // 4. hpValStr
                std::string hpColor = "";
                paintStrLeft(drawX, hpY, hpValStr, hpColor);
            }
        }

        // Texto flutuante de combate (FCT)
        if (danoAmount > 0 && flashDanoInimigo > 0) {
            int fctY = startY - 3;
            if (fctY < 0) fctY = 0;
            
            std::string textFCT = isCura ? ("+" + std::to_string(danoAmount)) : ("-" + std::to_string(danoAmount));
            std::string colorFCT = "";
            
            int tremble = (!isCura && flashDanoInimigo % 2 == 0) ? 1 : -1;
            
            paintStr(startX + croppedWidth/2 + tremble, fctY, textFCT, colorFCT);
        }
    }


}


void RaycasterRendererCombate::renderizarQuadroD2D(
    std::vector<Pixel3D>& screen,
    int LARGURA_TELA,
    int ALTURA_TELA,
    Character* jogador, 
    const std::vector<Character*>& enemies,
    Character* alvoAnimacao,
    int frame,
    int framesDeDanoJogador,
    int danoAmount,
    bool isCura,
    int tempoMs,
    bool isMorte,
    const std::vector<std::string>& dropsAnimacao,
    float spriteOpacity
) {
    (void)jogador;
    (void)framesDeDanoJogador;
    std::vector<Character*> enemiesToRender;
    for (auto* e : enemies) {
        if (e && (e->obterVida() > 0 || (isMorte && e == alvoAnimacao) || e->obterMorteAnimada())) {
            enemiesToRender.push_back(e);
        }
    }

    int total = (int)enemiesToRender.size();
    for (size_t i = 0; i < enemiesToRender.size(); ++i) {
        auto enemy = enemiesToRender[i];
        int frameAnim = 0;
        int danoAnim = 0;
        bool curaAnim = false;
        bool isSel = false;
        if (enemy == alvoAnimacao) {
            frameAnim = frame;
            danoAnim = danoAmount;
            curaAnim = isCura;
        }

        sobreporSpriteD2D(
            screen, enemy, (int)i, total, LARGURA_TELA, ALTURA_TELA,
            frameAnim, danoAnim, curaAnim, tempoMs, isMorte && enemy == alvoAnimacao, frame, dropsAnimacao, isSel, spriteOpacity
        );
    }
}

void RaycasterRendererCombate::sobreporSpriteD2D(
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
    bool isMorte, 
    int frameMorte, 
    const std::vector<std::string>& dropsAnimacao, 
    bool isSelecionado,
    float spriteOpacity
) {
    (void)danoAmount;
    (void)dropsAnimacao;
    (void)isSelecionado;
    const std::vector<std::string>& arteOriginalInimigo = enemy->obterRaca()->getRaceAppearance();
    if (arteOriginalInimigo.empty()) return;

    double progress = 0.0;
    int totalFramesMorte = 0;
    if (isMorte) {
        totalFramesMorte = 12;
        progress = std::min(1.0, static_cast<double>(frameMorte) / totalFramesMorte);
    }

    auto [baseR, baseG, baseB] = obterCorSpriteInimigo(enemy);
    std::vector<std::string> arteUsada = arteOriginalInimigo;
    
    int alturaArte = static_cast<int>(arteUsada.size());
    float fator = 1.0f;
    RaceType tipo = enemy->obterRaceType();
    bool isBoss = enemy->isBoss();
    
    switch (tipo) {
        case RaceType::Ork:
        case RaceType::OrkExilado: fator = 2.7f; break;
        case RaceType::Goblin: fator = 2.5f; break;
        case RaceType::Slime: fator = 2.5f; break;
        case RaceType::Mahoraga: fator = 3.0f; break;
        case RaceType::AbominacaoFloresta: fator = 1.5f; break;
        case RaceType::Troll: fator = 1.9f; break;
        case RaceType::Mimic: fator = 2.5f; break;
        case RaceType::Fairy: fator = 3.2f; break;
        default: break;
    }
    
    if (!isBoss) {
        size_t h = reinterpret_cast<size_t>(enemy);
        float pct = ((h % 101) - 50.0f) / 1000.0f;
        fator *= (1.0f + pct);
    }
    
    fator *= (alturaTela / 40.0f);
    
    if (alturaArte > 10) {
        arteUsada.clear();
        alturaArte = static_cast<int>(arteUsada.size());
    }
    
    int reservedBottom = (int)(14 * (alturaTela / 40.0f));
    int larguraArte = 0;
    for (const auto& linha : arteUsada) {
        int comp = (int)linha.length();
        if (comp > larguraArte) larguraArte = comp;
    }

    int swayOff = 0;
    if (!isMorte) {
        int stepSway = (tempoMs / 200) % 8;
        if (stepSway < 0) stepSway += 8;
        int swayPattern[] = {0, 1, 2, 1, 0, -1, -2, -1};
        swayOff = swayPattern[stepSway] * (int)(2 * (alturaTela / 40.0f));
    }
    
    int maxStartY = alturaTela - (int)(alturaArte * fator) - reservedBottom;
    int startY = (alturaTela - (int)(alturaArte * fator)) / 2;
    startY += alturaTela / 10;
    if (startY > maxStartY) startY = maxStartY;
    if (startY < 0) startY = 0;
    
    int startX = 0;
    
    auto obterTgtRGB = [&](char c, int ry) -> std::tuple<int,int,int> {
        int currentBaseR = baseR;
        int currentBaseG = baseG;
        int currentBaseB = baseB;
        if (tipo == RaceType::Mahoraga && ry * fator < 24) {
            currentBaseR = 255; currentBaseG = 215; currentBaseB = 0;
        }
        int rMod = currentBaseR, gMod = currentBaseG, bMod = currentBaseB;
        if (c == '@' || c == 'M' || c == 'W' || c == '#' || c == '&' || c == '8') { rMod = (int)(currentBaseR * 0.4f); gMod = (int)(currentBaseG * 0.4f); bMod = (int)(currentBaseB * 0.4f); }
        else if (c == '%' || c == 'O' || c == 'X' || c == 'S' || c == 'Q') { rMod = (int)(currentBaseR * 0.6f); gMod = (int)(currentBaseG * 0.6f); bMod = (int)(currentBaseB * 0.6f); }
        else if (c == '*' || c == '+' || c == 'x' || c == 'o' || c == '=' || c == 'H') { rMod = (int)(currentBaseR * 0.8f); gMod = (int)(currentBaseG * 0.8f); bMod = (int)(currentBaseB * 0.8f); }
        else if (c == '.' || c == ',' || c == ':' || c == ';' || c == '-' || c == '~') { rMod = (int)(currentBaseR * 1.2f); gMod = (int)(currentBaseG * 1.2f); bMod = (int)(currentBaseB * 1.2f); }
        else if (c == ' ' || c == '	') { rMod = -1; gMod = -1; bMod = -1; }
        
        if (flashDanoInimigo > 0 && flashDanoInimigo % 2 == 0) {
            if (isCura) { rMod = 50; gMod = 255; bMod = 50; }
            else { rMod = 255; gMod = 50; bMod = 50; }
        }
        
        if (rMod > 255) rMod = 255; if (gMod > 255) gMod = 255; if (bMod > 255) bMod = 255;
        return {rMod, gMod, bMod};
    };

    int cropOffset = 0;
    if (isMorte) {
        int arteHeightScaled = (int)(alturaArte * fator);
        cropOffset = (int)(arteHeightScaled * progress);
    }
    
    int espacamento = larguraTela / (totalInimigos + 1);
    int centroX = espacamento * (inimigoIdx + 1);
    int halfCroppedWidth = (int)((larguraArte * fator * (isMorte ? (1.0 - progress) : 1.0)) / 2);
    startX = centroX - halfCroppedWidth + swayOff;
    
    for (int y = 0; y < alturaArte; ++y) {
        if (y >= (int)arteUsada.size()) continue;
        const std::string& linhaDaArte = arteUsada[y];
        
        for (int rowSub = 0; rowSub < (int)fator; ++rowSub) {
            int drawY = startY + (int)(y * fator) + rowSub + cropOffset;
            if (drawY < 0 || drawY >= alturaTela) continue;
            
            for (int x = 0; x < larguraArte; ++x) {
                if (x >= (int)linhaDaArte.length()) break;
                char c = linhaDaArte[x];
                
                auto [tgtR, tgtG, tgtB] = obterTgtRGB(c, y);
                if (tgtR == -1) continue;
                
                for (int colSub = 0; colSub < (int)fator; ++colSub) {
                    int drawX = startX + (int)(x * fator) + colSub;
                    if (isMorte) drawX += (int)((rand() % 5 - 2) * progress * 5);
                    
                    if (drawX < 0 || drawX >= larguraTela) continue;
                    
                    int idx = drawY * larguraTela + drawX;
                    if (idx < 0 || idx >= (int)screen.size()) continue;
                    
                    Pixel3D& p = screen[idx];
                    p.r = static_cast<int>(p.r * (1.0f - spriteOpacity) + tgtR * spriteOpacity);
                    p.g = static_cast<int>(p.g * (1.0f - spriteOpacity) + tgtG * spriteOpacity);
                    p.b = static_cast<int>(p.b * (1.0f - spriteOpacity) + tgtB * spriteOpacity);
                    p.hasFg = false; // Sobrescreve o caractere com uma cor solida
                }
            }
        }
    }
}

