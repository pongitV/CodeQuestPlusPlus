#pragma once
#include <cstdint>
#include <mutex>

struct CorRGB {
    uint8_t r, g, b;
};

enum class TexID {
    Nenhuma = 0,
    // Paredes
    LabirintoMadeira,
    LabirintoArcoPilar,
    LabirintoArcoFundo,
    MorganaMadeira,
    IgrejaVitral,
    PonteMadeira,
    Alchemist,
    EntradaIgreja,
    ManequimAnok,
    Franchesco,
    Bjorn,
    Cavaleiro,
    ReinoMadeira,
    IgrejaAltar,
    IgrejaParede,
    IgrejaParedeAltar,
    IgrejaTeto,
    PatioMuro,
    FlorestaEstrutura,
    PadraoEstrutura,
    ArvoreCoracao,
    ArvoreFloresta,
    PedraVila,
    PedraSpawn,
    SalaChefeParede,
    CavernaCoracaoParede,
    ParedeInvalida,
    
    // Chaos e Tetos
    ChaoLabirintoBorda,
    ChaoLabirinto,
    ChaoSalaChefeFora,
    ChaoSalaChefeDentro,
    ChaoCoracaoMusgo,
    ChaoCoracaoTerra,
    ChaoCoracaoEscuro,
    ChaoGramaFloresta,
    ChaoGramaVila,
    ChaoTerra,
    ChaoPadrao,
    TetoIndoorsCoracaoMusgo,
    TetoIndoorsCoracaoMadeira,
    TetoIndoorsCoracaoEscuro,
    TetoIndoorsPadrao
};

#include <string>

class GerenciadorTexturas {
public:
    static void initialize();
    static CorRGB obterCor(TexID id, int tx, int ty);
    
    // English Aliases
    static CorRGB getColor(TexID id, int tx, int ty) { return obterCor(id, tx, ty); }

    // Lookup tables para otimizacao de funcoes trigonometricas
    static float fastSin(float angle);

    static float fastCos(float angle);

private:
    static std::once_flag initFlag;
    static bool inicializado;
    static CorRGB cache[256][16384];
    static float tabelaSin[4096];

    static void gerar(TexID id);
    static void carregarPNGEmCache(TexID id, const std::string& caminhoAsset);
};

using TextureManager = GerenciadorTexturas;

#include <algorithm>

inline CorRGB GerenciadorTexturas::obterCor(TexID id, int tx, int ty) {
    if (!inicializado) initialize();
    constexpr int maxIndex = 127;
    tx = std::clamp(tx, 0, maxIndex);
    ty = std::clamp(ty, 0, maxIndex);
    return cache[static_cast<int>(id)][ty * 128 + tx];
}
