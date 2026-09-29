#include "RaycasterWorld.h"
#include "TextureManager.h"
#include "MapCache.h"
#include "Illuminator.h"
#include "SkyRenderer.h"
#include <algorithm>
#include <cmath>
#include <string_view>
#include "../../../ui/UIManager.h"

#include "../../../maps/village/Map1VillageLayout.h"
#include "../../../maps/forest/Map2ForestLayout.h"
#include "../../../maps/kingdom/Map3KingdomBridgeLayout.h"
#include "../../../maps/kingdom/Map4KingdomLayout.h"

#include <atomic>

static std::atomic<size_t> g_currentMapHash{0};

char RaycasterWorld::obterNPCProximo(const std::string& tituloMapa, int mapX, int mapY, const std::vector<std::string>* matrizDoMapa) {
    if (matrizDoMapa != nullptr) {
        return MapCachea::obterNPCProximo(tituloMapa, mapX, mapY, *matrizDoMapa);
    }
    
    static thread_local std::string lastTitulo;
    static thread_local std::vector<std::string> layout;

    if (tituloMapa != lastTitulo) {
        lastTitulo = tituloMapa;
        std::string upper = tituloMapa;
        for (char& c : upper) c = std::toupper(static_cast<unsigned char>(c));

        if (upper == "REINO" || upper == "PATIO DO REINO") layout = Mapa4ReinoLayouts::obterLayoutReino();
        else if (upper.find("IGREJA") != std::string::npos) layout = Mapa4ReinoLayouts::obterLayoutIgreja();
        else if (upper.find("PONTE") != std::string::npos || upper == "CAMINHO DO REINO") layout = Mapa3PonteReinoLayouts::obterLayoutPonteReino();
        else if (upper.find("VILA") != std::string::npos) layout = Mapa1VilaLayouts::obterLayoutVilaInicial();
        else if (upper.find("FLORESTA") != std::string::npos) layout = Mapa2FlorestaLayouts::obterLayoutFloresta();
        else if (upper.find("CAVERNA") != std::string::npos) layout = Mapa1VilaLayouts::obterLayoutCaverna(false);
        else layout.clear();
    }

    return MapCachea::obterNPCProximo(tituloMapa, mapX, mapY, layout);
}

static thread_local std::string g_currentMapTitle = "";

static const MapFlags& obterFlagsMapa(const std::string& tituloMapa) {
    static thread_local std::string lastTitle;
    if (tituloMapa != lastTitle) {
        lastTitle = tituloMapa;
        g_currentMapTitle = tituloMapa;
    }
    return MapCachea::obterFlags(tituloMapa);
}


bool RaycasterWorld::isTemaFloresta(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return (flags.tituloUpper.find("FLORESTA") != std::string::npos || flags.tituloUpper.find("BOSQUE") != std::string::npos || flags.tituloUpper.find("CABANA") != std::string::npos);
}

bool RaycasterWorld::isEntity(char c) {
    if (c == 'T' && g_currentMapTitle.find("CORACAO") != std::string::npos) return false;
    if (c == 'C' && (g_currentMapTitle.find("VILA") != std::string::npos || g_currentMapTitle.find("INICIO") != std::string::npos)) return false;
    switch (c) {
        case 'G': case 'O': case 'B': case 'F': case 'S': case 'A':
        case 'M': case 'T': case 'H': case 'R': case 'P': case '^':
        case '*': case 'C': case 'I': case 'Q': case 'Y': case 'Z':
        case 'V': case 'W': case 'N':
            return true;
        default:
            return false;
    }
}

bool RaycasterWorld::isTeleport(char c) { return c == '^'; }

bool RaycasterWorld::isWalkable(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    char c = matrizDoMapa[mapY][mapX];
    if (c == '*') return false;
    
    if (c == '=' || c == '|' || c == '\'' || c == '+') return false;

    if (c == '.' || c == ' ' || c == '^' || c == '~' || c == 'C' || isEntity(c)) return true;
    if (isMapLabel(mapX, mapY, matrizDoMapa)) return true;
    return false;
}

void RaycasterWorld::atualizarMapHash(const std::vector<std::string>& matrizDoMapa) {
    size_t hash = 0;
    for (const auto& r : matrizDoMapa) {
        hash ^= std::hash<std::string_view>{}(r) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    g_currentMapHash = hash;
}

size_t RaycasterWorld::obterMapHash() {
    return g_currentMapHash;
}

bool RaycasterWorld::isMapLabel(int mapX, int mapY, const std::vector<std::string>& matrizDoMapa) {
    static thread_local size_t lastMapHash = 0;
    static thread_local std::vector<std::vector<char>> cachedLabels;

    int height = matrizDoMapa.size();
    if (height == 0) return false;
    int width = matrizDoMapa[0].size();
    if (mapY < 0 || mapY >= height || mapX < 0 || mapX >= width) return false;

    size_t hash = g_currentMapHash;
    if (hash == 0) {
        for (const auto& r : matrizDoMapa) {
            hash ^= std::hash<std::string_view>{}(r) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
    }

    if (hash != lastMapHash) {
        lastMapHash = hash;
        cachedLabels.assign(height, std::vector<char>(width, 2));
    }

    if (cachedLabels[mapY][mapX] != 2) {
        return cachedLabels[mapY][mapX] == 1;
    }

    char c = matrizDoMapa[mapY][mapX];
    bool result = false;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        if (isEntity(c)) {
            bool hasAdjacentText = false;
            for (int dx = -1; dx <= 1; dx += 2) {
                int nx = mapX + dx;
                if (mapY >= 0 && mapY < height && nx >= 0 && nx < (int)matrizDoMapa[mapY].size()) {
                    char adj = matrizDoMapa[mapY][nx];
                    if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                        hasAdjacentText = true;
                        break;
                    }
                }
            }
            if (!hasAdjacentText) {
                for (int dy = -1; dy <= 1; dy += 2) {
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < height && mapX >= 0 && mapX < (int)matrizDoMapa[ny].size()) {
                        char adj = matrizDoMapa[ny][mapX];
                        if (adj == '^') {
                            hasAdjacentText = true;
                            break;
                        }
                    }
                }
            }
            result = hasAdjacentText;
        } else {
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = mapX + dx;
                    int ny = mapY + dy;
                    if (ny >= 0 && ny < height && nx >= 0 && nx < (int)matrizDoMapa[ny].size()) {
                        char adj = matrizDoMapa[ny][nx];
                        if (adj == '^' || (adj >= 'a' && adj <= 'z')) {
                            result = true;
                            break;
                        }
                    }
                }
                if (result) break;
            }
        }
    }
    cachedLabels[mapY][mapX] = result ? 1 : 0;
    return result;
}

Pixel3D RaycasterWorld::obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const Illuminator::InfoLuz& infoLuz, float hitX, float hitY, bool isSideWall, char npcEncontrado, float nx, float ny) {
    (void)isSideWall; (void)distanciaAteParede; (void)profundidadeMaxima; (void)tempoAnimacao;
    const auto& flags = obterFlagsMapa(tituloMapa);
    int alturaParede = chao - teto;
    
    float texY = 0.0f;
    if (alturaParede > 0) texY = (float)(y - teto) / (float)alturaParede;
    if (texY > 0.999f) texY = 0.999f;
    int tx = (int)(texX * 128.0f) % 128;
    int ty = (int)(texY * 128.0f) % 128;

    bool isReino = flags.isReino;
    bool isEstrutura = (charParede == '|' || charParede == '_' || charParede == '[' || charParede == ']' || charParede == '{' || charParede == '}' || charParede == '/' || charParede == '\\' || charParede == '<' || charParede == '>' || charParede == ';' || charParede == '=' || charParede == '-' || charParede == ':' || charParede == '+');
    bool isLabyrinthArch = (!isReino && temaFloresta && charParede == '#' && hitX >= 125.0f && hitX <= 150.0f && hitY >= 5.0f && hitY <= 15.0f);

    TexID texID = TexID::ParedeInvalida;

    if (flags.isLabirinto) {
        texID = TexID::LabirintoMadeira;
    } else if (isLabyrinthArch) {
        int mapX = (int)hitX;
        int mapY = (int)hitY;
        bool isPilar = (std::abs(mapY - 12) == 1 && mapX >= 132 && mapX <= 136);
        if (isPilar) texID = TexID::LabirintoArcoPilar;
        else texID = TexID::LabirintoArcoFundo;
    } else if (isEstrutura && npcEncontrado == 'M') {
        texID = TexID::MorganaMadeira;
    } else if (isReino && (isEstrutura || charParede == '#' || charParede == '+')) {
        if (charParede == '+') {
            if (flags.isIgreja) texID = TexID::IgrejaParedeAltar;
        } else if (charParede == '|') {
            if (flags.isIgreja) texID = TexID::IgrejaVitral;
            else if (flags.isPonte) texID = TexID::PonteMadeira;
            else {
                if (npcEncontrado == 'Q') texID = TexID::Alchemist;
                else if (npcEncontrado == 'I' || npcEncontrado == 'P') texID = TexID::EntradaIgreja;
                else if (npcEncontrado == 'A' || npcEncontrado == 'N') texID = TexID::ManequimAnok;
                else if (npcEncontrado == 'F') texID = TexID::Franchesco;
                else if (npcEncontrado == 'B') texID = TexID::Bjorn;
                else if (npcEncontrado == 'C') texID = TexID::Cavaleiro;
                else texID = TexID::ReinoMadeira;
            }
        } else {
            if (flags.isIgreja) {
                if (hitX < 10.0f) texID = TexID::IgrejaAltar;
                else texID = TexID::IgrejaParede;
            } else {
                bool isBattlementGap = (ty < 12 && (tx % 32) >= 16);
                if (isBattlementGap) {
                    Pixel3D px;
                    px.isFundo = true;
                    return px;
                }
                texID = TexID::PatioMuro;
            }
        }
    } else if (isEstrutura) {
        if (temaFloresta) texID = TexID::FlorestaEstrutura;
        else texID = TexID::PadraoEstrutura;
    } else if (!isReino && (temaFloresta || flags.isTerra) && charParede == 'T') {
        if (flags.isCoracao) texID = TexID::ArvoreCoracao;
        else texID = TexID::ArvoreFloresta;
    } else if (charParede == '*') {
        if (flags.isCoracao) texID = TexID::ArvoreCoracao;
        else texID = TexID::ArvoreFloresta;
    } else if (charParede == '#') {
        if (flags.isSpawn) texID = TexID::PedraSpawn;
        else if (flags.isFloresta) texID = TexID::FlorestaEstrutura;
        else if (flags.isCaverna) {
            if (flags.isCoracao) texID = TexID::CavernaCoracaoParede;
            else texID = TexID::PedraVila;
        } else if (flags.isSalaChefe) texID = TexID::SalaChefeParede;
        else texID = TexID::PedraVila;
    } else if (charParede == 'T') {
        if (flags.isIgreja) texID = TexID::IgrejaTeto;
        else texID = TexID::PadraoEstrutura; // Default fallback for T
    } else if (!isReino) {
        if (flags.isSpawn) texID = TexID::PedraSpawn;
        else if (flags.isSalaChefe) texID = TexID::SalaChefeParede;
        else if (flags.isCaverna) {
            if (flags.isCoracao) texID = TexID::CavernaCoracaoParede;
            else texID = TexID::PedraVila;
        } else {
            if (flags.isFloresta) texID = TexID::FlorestaEstrutura;
            else texID = TexID::PedraVila;
        }
    } else {
        if (flags.isFloresta) texID = TexID::FlorestaEstrutura;
        else texID = TexID::PedraVila;
    }

    CorRGB cor = GerenciadorTexturas::obterCor(texID, tx, ty);
    return Illuminator::aplicarLuzPrecalculada(cor.r, cor.g, cor.b, infoLuz, false, true, nx, ny);
}
Pixel3D RaycasterWorld::obterPixelParedeInternal(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, bool isSideWall, char npcEncontrado, float nx, float ny) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    Illuminator::InfoLuz info = Illuminator::calcularInfoLuz(distanciaAteParede * 0.55f, profundidadeMaxima, flags.temaCeu, luzes, hitX, hitY, nullptr, tempoAnimacao);
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, info, hitX, hitY, isSideWall, npcEncontrado, nx, ny);
}

Pixel3D RaycasterWorld::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const Illuminator::InfoLuz& infoLuz, float hitX, float hitY, char npcEncontrado, float nx, float ny) {
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, infoLuz, hitX, hitY, isSideWall, npcEncontrado, nx, ny);
}

Pixel3D RaycasterWorld::obterPixelParede(const std::string& tituloMapa, bool temaFloresta, float distanciaAteParede, float profundidadeMaxima, char charParede, int y, int teto, int chao, float texX, float tempoAnimacao, bool isSideWall, const std::vector<std::tuple<int, int, int>>& luzes, float hitX, float hitY, char npcEncontrado, float nx, float ny) {
    return obterPixelParedeInternal(tituloMapa, temaFloresta, distanciaAteParede, profundidadeMaxima, charParede, y, teto, chao, texX, tempoAnimacao, luzes, hitX, hitY, isSideWall, npcEncontrado, nx, ny);
}


Pixel3D RaycasterWorld::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const std::vector<std::tuple<int, int, int>>& luzes, const std::vector<std::string>* matrizDoMapa, float tempoAnimacao) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    int temaCeu = flags.temaCeu;
    currentDist *= 0.55f;

    bool isTerra = flags.isTerra;
    bool isLabirinto = flags.isLabirinto;
    bool isSalaChefe = flags.isSalaChefe;
    bool isCoracao = flags.tituloUpper.find("CORACAO") != std::string::npos;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 128.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 128.0f));
    int tx = globX & 127;
    int ty = globY & 127;

    TexID texID = TexID::ChaoPadrao;
    char c = ' ';
    uint8_t fgR = 0, fgG = 0, fgB = 0;
    uint8_t bgR = 0, bgG = 0, bgB = 0;
    bool isProcedural = false;

    if (isLabirinto) {
        fgR = 150; fgG = 130; fgB = 90;
        bool bordaX = ((globX & 127) < 2) || ((globX & 127) > 125);
        bool bordaY = ((globY & 31) < 2) || ((globY & 31) > 29);
        if (bordaX || bordaY) texID = TexID::ChaoLabirintoBorda;
        else texID = TexID::ChaoLabirinto;
    } else if (isSalaChefe) {
        float cx = (globX & 127) - 64.0f;
        float cy = (globY & 127) - 64.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = GerenciadorTexturas::fastSin(dist * 0.4f - angle * 3.0f);
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        if (spiral > 0.0f) texID = TexID::ChaoSalaChefeDentro;
        else texID = TexID::ChaoSalaChefeFora;
    } else if (isCoracao) {
        float cx = (globX & 255) - 128.0f;
        float cy = (globY & 255) - 128.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = GerenciadorTexturas::fastSin(dist * 0.2f + angle * 4.0f + globX * 0.1f);
        bool hasMoss = ((globX * 17 + globY * 13) % 100) < 40 || (spiral > 0.5f);
        if (hasMoss) { texID = TexID::ChaoCoracaoMusgo; fgR = 30; fgG = 80; fgB = 20; }
        else if (spiral > 0.0f) { texID = TexID::ChaoCoracaoTerra; fgR = 50; fgG = 30; fgB = 15; }
        else { texID = TexID::ChaoCoracaoEscuro; fgR = 25; fgG = 15; fgB = 10; }
    } else if (flags.isReino || flags.isPonte) {
        texID = flags.isPonte ? TexID::PonteMadeira : TexID::ChaoPadrao;
        isProcedural = false;
    } else if (flags.isCaverna && !isCoracao) {
        float cx = globX * 0.06f;
        float cy = globY * 0.06f;
        float cx2 = (globX - globY) * 0.03f;
        float noise = GerenciadorTexturas::fastSin(cx) + GerenciadorTexturas::fastSin(cy) + GerenciadorTexturas::fastSin(cx2);
        
        int var = (int)(noise * 5.0f);
        bgR = 30 + var; bgG = 30 + var; bgB = 30 + var;
        c = ' ';
        isProcedural = true;
    } else if (isTerra) {
        float cx = globX * 0.123f;
        float cy = globY * 0.091f;
        float cx2 = (globX + globY) * 0.054f;
        float cy2 = ((float)globX - globY) * 0.111f;
        float noise = GerenciadorTexturas::fastSin(cx) + GerenciadorTexturas::fastSin(cy) + GerenciadorTexturas::fastSin(cx2) + GerenciadorTexturas::fastSin(cy2);
        bool isGrama = (noise > -3.0f);
        if (isGrama) {
            if (flags.tituloUpper.find("FLORESTA") != std::string::npos) { fgR = 6; fgG = 35; fgB = 6; texID = TexID::ChaoGramaFloresta; }
            else { fgR = 12; fgG = 75; fgB = 12; texID = TexID::ChaoGramaVila; }
        } else {
            fgR = 45; fgG = 25; fgB = 10; texID = TexID::ChaoTerra;
        }
    } else {
        fgR = 60; fgG = 60; fgB = 60; texID = TexID::ChaoPadrao;
        if (((globX * 17 + globY * 23) & 63) < 4) c = '.';
        else if (((globX * globX + globY * 13) & 63) < 3) c = '-';
        else if (((globX * 3 + globY * 7) & 31) < 2) c = '`';
    }
    
    CorRGB cor;
    if (isProcedural) {
        cor.r = bgR; cor.g = bgG; cor.b = bgB;
    } else {
        cor = GerenciadorTexturas::obterCor(texID, tx, ty);
    }
    
    Pixel3D px = Illuminator::aplicarNevoa(cor.r, cor.g, cor.b, currentDist, profundidadeMaxima, temaCeu, luzes, currentX, currentY, false, matrizDoMapa, false, 0.0f, 0.0f, tempoAnimacao);
    if (c != ' ' && currentDist <= profundidadeMaxima * 0.5f) {
        px.ch = c;
        px.fgR = fgR;
        px.fgG = fgG;
        px.fgB = fgB;
        px.hasFg = true;
    }
    return px;
}
static CorRGB obterPixelChaoAntiRepeticao(TexID texID, float currentX, float currentY, unsigned int globX, unsigned int globY) {
    float warpX = GerenciadorTexturas::fastSin(currentX * 0.5f + currentY * 0.4f) * 14.0f;
    float warpY = GerenciadorTexturas::fastSin(currentY * 0.5f - currentX * 0.4f) * 14.0f;
    
    int sampleX = static_cast<int>(globX + warpX);
    int sampleY = static_cast<int>(globY + warpY);
    if (sampleX < 0) sampleX = -sampleX;
    if (sampleY < 0) sampleY = -sampleY;

    int tileX = sampleX / 128;
    int tileY = sampleY / 128;
    unsigned int hash = (tileX * 1597334677U + tileY * 3812015801U);
    
    int tx = sampleX & 127;
    int ty = sampleY & 127;
    
    if ((hash & 1) != 0) tx = 127 - tx;
    if ((hash & 2) != 0) ty = 127 - ty;

    CorRGB cor = GerenciadorTexturas::obterCor(texID, tx, ty);

    float macroShade = GerenciadorTexturas::fastSin(currentX * 0.12f) * GerenciadorTexturas::fastSin(currentY * 0.12f);
    float factor = 1.0f + (macroShade * 0.18f);

    int r = std::clamp((int)(cor.r * factor), 0, 255);
    int g = std::clamp((int)(cor.g * factor), 0, 255);
    int b = std::clamp((int)(cor.b * factor), 0, 255);

    return { static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b) };
}

Pixel3D RaycasterWorld::obterPixelChao(const std::string& tituloMapa, float currentX, float currentY, float currentDist, float profundidadeMaxima, const Illuminator::InfoLuz& infoLuz) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    currentDist *= 0.55f;

    bool isTerra = flags.isTerra;
    bool isLabirinto = flags.isLabirinto;
    bool isSalaChefe = flags.isSalaChefe;
    bool isCoracao = flags.tituloUpper.find("CORACAO") != std::string::npos;

    unsigned int globX = static_cast<unsigned int>(std::abs(currentX * 128.0f));
    unsigned int globY = static_cast<unsigned int>(std::abs(currentY * 128.0f));

    char c = ' ';
    int r = 0, g = 0, b = 0;
    uint8_t fgR = 0, fgG = 0, fgB = 0;

    if (isLabirinto) {
        fgR = 150; fgG = 130; fgB = 90;
        bool bordaX = ((globX & 127) < 2) || ((globX & 127) > 125);
        bool bordaY = ((globY & 31) < 2) || ((globY & 31) > 29);
        if (bordaX || bordaY) {
            r = 40; g = 40; b = 30;
            c = ' ';
        } else {
            if (((globX + globY) & 1) == 0) { r = 180; g = 160; b = 110; }
            else                          { r = 160; g = 140; b = 95; }
            c = ' ';
        }
    } else if (isSalaChefe) {
        float cx = (globX & 127) - 64.0f;
        float cy = (globY & 127) - 64.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.4f - angle * 3.0f);

        r = 5; g = 5; b = 5;
        fgR = 50; fgG = 50; fgB = 50; 
        if (spiral > 0.3f) c = '@';
        else if (spiral > 0.0f) c = '%';
        else if (spiral > -0.3f) c = '.';
        else c = ' ';
    } else if (isCoracao) {
        // Chao de musgo e terra para Coracao da Forest
        float cx = (globX & 255) - 128.0f;
        float cy = (globY & 255) - 128.0f;
        float dist = std::sqrt(cx*cx + cy*cy);
        float angle = std::atan2(cy, cx);
        float spiral = std::sin(dist * 0.2f + angle * 4.0f + globX * 0.1f);
        
        bool hasMoss = ((globX * 17 + globY * 13) % 100) < 40 || (spiral > 0.5f);
        
        if (hasMoss) {
            r = 30; g = 80; b = 20; // Verde musgo
            fgR = 30; fgG = 80; fgB = 20;
        } else if (spiral > 0.0f) {
            r = 50; g = 30; b = 15; // Madeira/Terra marrom
            fgR = 50; fgG = 30; fgB = 15;
        } else {
            r = 25; g = 15; b = 10; // Madeira/Terra escura
            fgR = 25; fgG = 15; fgB = 10;
        }
        c = ' ';
    } else if (flags.isReino || flags.isPonte) {
        float cx = globX * 0.15f;
        float cy = globY * 0.15f;
        float noise = std::sin(cx) * std::sin(cy);
        
        int row = globY / 24;
        int offset = (row % 2 == 0) ? 0 : 12;
        float blockX = ((globX + offset) % 24) / 24.0f;
        float blockY = (globY % 24) / 24.0f;
        
        bool bordaTile = (blockX < 0.1f || blockX > 0.9f || blockY < 0.1f || blockY > 0.9f);
        
        if (bordaTile) {
            r = 20; g = 20; b = 22; 
        } else {
            int var = (int)(noise * 6.0f);
            r = 38 + var; g = 40 + var; b = 43 + var; 
        }
        c = ' ';
    } else if (flags.isCaverna && !isCoracao) {
        float cx = globX * 0.06f;
        float cy = globY * 0.06f;
        float cx2 = (globX - globY) * 0.03f;
        float noise = std::sin(cx) + std::sin(cy) + std::sin(cx2);
        
        int var = (int)(noise * 5.0f);
        r = 30 + var; g = 30 + var; b = 30 + var;
        c = ' ';
    } else if (flags.isFloresta || flags.tituloUpper.find("FLORESTA") != std::string::npos || flags.tituloUpper.find("BOSQUE") != std::string::npos) {
        CorRGB cor = obterPixelChaoAntiRepeticao(TexID::ChaoGramaFloresta, currentX, currentY, globX, globY);
        r = cor.r;
        g = cor.g;
        b = cor.b;
        c = ' ';
    } else if (isTerra || flags.isSpawn || flags.tituloUpper.find("VILA") != std::string::npos || flags.tituloUpper.find("INICIO") != std::string::npos) {
        CorRGB cor = obterPixelChaoAntiRepeticao(TexID::ChaoGramaVila, currentX, currentY, globX, globY);
        r = cor.r;
        g = cor.g;
        b = cor.b;
        c = ' ';
    } else {
        fgR = 60; fgG = 60; fgB = 60;
        if (((globX + globY) & 1) == 0) { r = 24; g = 24; b = 24; }
        else if (((globX * 3 + globY * 5) & 7) < 2) { r = 16; g = 16; b = 16; }
        else { r = 20; g = 20; b = 20; }
        
        if (((globX * 17 + globY * 23) & 63) < 4) c = '.';
        else if (((globX * globX + globY * 13) & 63) < 3) c = '-';
        else if (((globX * 3 + globY * 7) & 31) < 2) c = '`';
    }
    
    Illuminator::InfoLuz infoLocal = infoLuz;
    infoLocal.nevoaPercent = std::min(1.0f, (currentDist / (profundidadeMaxima * 0.8f)) *
                                             (currentDist / (profundidadeMaxima * 0.8f)));
    Pixel3D px = Illuminator::aplicarLuzPrecalculada(r, g, b, infoLocal);
    if (c != ' ' && currentDist <= profundidadeMaxima * 0.5f) {
        px.ch = c;
        px.fgR = fgR;
        px.fgG = fgG;
        px.fgB = fgB;
        px.hasFg = true;
    }
    return px;
}

Pixel3D RaycasterWorld::obterPixelAgua(float currentX, float currentY, float currentDist, float profundidadeMaxima, float raioAngulo, float tempoAnimacao, int temaCeu) {
    int baseR=0, baseG=0, baseB=0;
    currentDist *= 0.55f;

    float waveX = GerenciadorTexturas::fastSin(currentX * 4.0f + tempoAnimacao * 2.0f);
    float waveY = GerenciadorTexturas::fastCos(currentY * 4.0f + tempoAnimacao * 1.5f);
    float wave = (waveX + waveY) * 0.5f; 

    if (wave > 0.3f) {
        baseR = 100; baseG = 200; baseB = 255;
    } else if (wave > -0.3f) {
        baseR = 60; baseG = 160; baseB = 235;
    } else {
        baseR = 30; baseG = 130; baseB = 215;
    }
    
    float angleOffset = wave * 0.2f; 
    float angReflexo = raioAngulo + angleOffset;
    while (angReflexo >= 2.0f * 3.14159f) angReflexo -= 2.0f * 3.14159f;
    while (angReflexo < 0) angReflexo += 2.0f * 3.14159f;
    
    if (angReflexo < 0.3f || angReflexo > (2.0f * 3.14159f - 0.3f)) {
        float dif = (angReflexo < 0.3f) ? angReflexo : ((2.0f * 3.14159f) - angReflexo);
        float intensidadeReflexo = 1.0f - (dif / 0.3f);
        intensidadeReflexo *= (0.5f + (wave + 1.0f) * 0.25f); 
        
        baseR = std::min(255, baseR + (int)(155 * intensidadeReflexo));
        baseG = std::min(255, baseG + (int)(95 * intensidadeReflexo));
        if (temaCeu != 1 && temaCeu != 2) baseB = std::min(255, baseB + (int)(255 * intensidadeReflexo)); 
    }

    std::vector<std::tuple<int, int, int>> noLuzes;
    Illuminator::InfoLuz info = Illuminator::calcularInfoLuz(currentDist, profundidadeMaxima, temaCeu, noLuzes, currentX, currentY, nullptr, tempoAnimacao);
    
    float nightFactor = std::max(0.4f, info.solIntensidade);
    baseR = (int)(baseR * nightFactor);
    baseG = (int)(baseG * nightFactor);
    baseB = (int)(baseB * nightFactor);
    
    float nf = info.nevoaPercent;
    float finalR = baseR * (1.0f - nf) + info.fogR * nf;
    float finalG = baseG * (1.0f - nf) + info.fogG * nf;
    float finalB = baseB * (1.0f - nf) + info.fogB * nf;
    
    Pixel3D px;
    px.r = (uint8_t)std::min(255, std::max(0, (int)finalR));
    px.g = (uint8_t)std::min(255, std::max(0, (int)finalG));
    px.b = (uint8_t)std::min(255, std::max(0, (int)finalB));
    px.ch = ' ';
    return px;
}
int RaycasterWorld::obterTemaCeu(const std::string& tituloMapa) {
    const auto& flags = obterFlagsMapa(tituloMapa);
    return flags.temaCeu;
}

bool RaycasterWorld::s_overrideCelestials = false;
float RaycasterWorld::s_overrideSunAng = 0.0f;
float RaycasterWorld::s_overrideSunRatioY = 0.0f;
float RaycasterWorld::s_overrideMoonAng = 0.0f;
float RaycasterWorld::s_overrideMoonRatioY = 0.0f;

Pixel3D RaycasterWorld::obterPixelTeto(int temaCeu, float raioAngulo, float anguloCeu, int y, int alturaTela, float tempoAnimacao, bool isMenu) {
    (void)anguloCeu; (void)tempoAnimacao; (void)isMenu;
    int horizonte = alturaTela / 2;
    if (temaCeu == 3) { 
        Pixel3D px;
        float ratioY = (horizonte > 0) ? (float)y / (float)horizonte : 1.0f;
        int tx = (int)(raioAngulo * 60.0f) % 128;
        int ty = (int)(ratioY * 60.0f) % 128;
        if (tx < 0) tx += 128;
        if (ty < 0) ty += 128;
        
        TexID texID = TexID::TetoIndoorsPadrao;
        bool isCoracao = g_currentMapTitle.find("CORACAO") != std::string::npos;
        bool isSalaChefe = g_currentMapTitle.find("CAVERNA") != std::string::npos || g_currentMapTitle.find("CHEFE") != std::string::npos;
        
        if (isCoracao) {
            float cx = (tx - 64.0f);
            float cy = (ty - 64.0f);
            float dist = std::sqrt(cx*cx + cy*cy);
            float angle = std::atan2(cy, cx);
            float spiral = GerenciadorTexturas::fastSin(dist * 0.2f + angle * 4.0f + tx * 0.1f);
            
            bool hasMoss = ((tx * 17 + ty * 13) % 100) < 20 || (spiral > 0.8f);
            
            if (hasMoss) texID = TexID::TetoIndoorsCoracaoMusgo;
            else if (spiral > 0.0f) texID = TexID::TetoIndoorsCoracaoMadeira;
            else texID = TexID::TetoIndoorsCoracaoEscuro;
        } else if (isSalaChefe) {
            texID = TexID::SalaChefeParede;
        }
        CorRGB cor = GerenciadorTexturas::obterCor(texID, tx, ty);
        px.r = cor.r; px.g = cor.g; px.b = cor.b;
        px.ch = ' '; px.isFundo = false;
        return px;
    }


    return SkyRenderer::calcularPixelCeu(anguloCeu, raioAngulo, y, horizonte, tempoAnimacao, isMenu);
}

char RaycasterWorld::obterSpriteChar(int /*mapX*/, int mapY, char c, const std::string& tituloMapa) {
    if (c == '^') {
        const auto& flags = obterFlagsMapa(tituloMapa);
        if (flags.tituloUpper.find("VILA") != std::string::npos) return '2';
        if (flags.tituloUpper.find("FLORESTA") != std::string::npos) {
            if (mapY > 15) return '1'; 
            return '5'; 
        }
        if (flags.tituloUpper.find("REINO") != std::string::npos) return '1';
        return '^';
    }

    if (c == '!' || c == '%') {
        return c; // Retorna ! ou % para serem desenhados como sprite pelo RaycasterRenderer (IDE)
    }
    if (c == '@') {
        return '@'; // Terminal hackeavel
    }
    if (c == 'Y' || c == '*') {
        return c;
    }
    const auto& flags = obterFlagsMapa(tituloMapa);
    if (flags.tituloUpper.find("IGREJA") != std::string::npos) {
        if (c == 'P') return 'J'; // Priest Benedito
    }

    if ((flags.tituloUpper.find("VILA") != std::string::npos || flags.tituloUpper.find("CASA") != std::string::npos) && c == 'F') {
        return 'V';
    }
    if (flags.tituloUpper.find("SALA DO CHEFE") != std::string::npos && (c == 'M' || c == 'A' || c == 'H' || c == 'O' || c == 'R' || c == 'G')) {
        return 'H';
    }
    if ((flags.tituloUpper.find("REINO") != std::string::npos || flags.tituloUpper.find("PATIO") != std::string::npos) && c == 'N') {
        return 'Z';
    }
    if ((flags.tituloUpper.find("CABANA") != std::string::npos || flags.tituloUpper.find("FLORESTA") != std::string::npos) && c == 'M') {
        return 'W';
    }
    if (flags.tituloUpper.find("LABIRINTO") != std::string::npos && c == 'B') {
        return 'X';
    }

    // Customizacoes para o PATIO DO REINO (e REINO) e Igreja
    if (flags.tituloUpper.find("PATIO DO REINO") != std::string::npos || flags.tituloUpper == "REINO") {
        if (c == 'F') return 'V'; // Franchesco
        if (c == 'N') return 'Z'; // Anok (Manequim)
        if (c == 'Q') return 'Q'; // Alchemist
        if (c == 'C') return 'C'; // Cavaleiro de Treino
        if (c == 'I') return 'Y'; // Igreja (Capela)
        if (c == 'B') return 'B'; // Bjorn
    }
    if (flags.tituloUpper.find("IGREJA") != std::string::npos) {
        if (c == 'P') return 'J'; // Priest Benedito
    }

    bool isReino = flags.isReino;
    if (isReino && flags.tituloUpper.find("PATIO DO REINO") == std::string::npos) {
        if (c == 'C' || c == 'G') return 'C';
    }
    return c;
}

std::string RaycasterWorld::obterCorMinimapaEntidade(char c, const std::string& tituloMapa) {
    return "";
}
