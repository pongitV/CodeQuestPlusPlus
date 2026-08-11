#pragma once

#include "UIRenderer.h"
#include "./screens/interfaces/IScreenManager.h"
#include "interfaces/IDiaryUI.h"
#include "interfaces/IInventoryUI.h"
#include "interfaces/IAttributesUI.h"
#include "interfaces/IBestiaryUI.h"
#include "interfaces/IScreenCombatUI.h"
#include "interfaces/IDefeatUI.h"
#include "interfaces/IVictoryUI.h"
#include "interfaces/IPauseUI.h"
#include "interfaces/IMapWorldUI.h"
#include <memory>

class GerenciadorPerspectiva {
public:
    static GerenciadorPerspectiva& obterInstancia() {
        static GerenciadorPerspectiva instancia;
        return instancia;
    }

    void initialize();
    void alternarVisao();
    bool isVisao3DAtiva() const;

    UIRenderer* obterRendererAtivo() const;
    IGerenciadorTelas* obterGerenciadorTelas() const;

    static IDiarioUI& obterDiarioUI();
    static IInventarioUI& obterInventarioUI();
    static IAtributosUI& obterAtributosUI();
    static IBestiarioUI& obterBestiarioUI();
    static ITelaCombateUI& obterTelaCombateUI();
    static IDefeatUI& obterDerrotaUI();
    static IVictoryUI& obterVitoriaUI();
    static IPauseUI& obterPauseUI();
    static IMapaMundoUI& obterMapaMundoUI();

    static float obterSensibilidadeMouseX();
    static float obterSensibilidadeMouseY();
    static void definirSensibilidadeMouse(float x, float y);

    // English Aliases
    static GerenciadorPerspectiva& getInstance() { return obterInstancia(); }
        void toggleView() { alternarVisao(); }
    bool is3DViewActive() const { return isVisao3DAtiva(); }
    static IDiarioUI& getDiaryUI() { return obterDiarioUI(); }
    static IInventarioUI& getInventoryUI() { return obterInventarioUI(); }
    static IAtributosUI& getAttributesUI() { return obterAtributosUI(); }
    static IBestiarioUI& getBestiaryUI() { return obterBestiarioUI(); }
    static ITelaCombateUI& getCombatUI() { return obterTelaCombateUI(); }
    static IPauseUI& getPauseUI() { return obterPauseUI(); }
    static IMapaMundoUI& getWorldMapUI() { return obterMapaMundoUI(); }

private:
    GerenciadorPerspectiva();
    ~GerenciadorPerspectiva() = default;

    GerenciadorPerspectiva(const GerenciadorPerspectiva&) = delete;
    GerenciadorPerspectiva& operator=(const GerenciadorPerspectiva&) = delete;

    bool m_visao3DAtiva;
    std::unique_ptr<UIRenderer> m_renderer3D;

    std::unique_ptr<IGerenciadorTelas> m_telas3D;
};

using PerspectiveManager = GerenciadorPerspectiva;
