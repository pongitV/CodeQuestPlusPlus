#include "MapInputController.h"
#include "../utils/MapHelper.h"
#include <windows.h>
#include "../../core/utils/InputDispatcher.h"
#include "../../core/utils/InputControl.h"
#include "../../ui/screens/pause/ScreenPause.h"
#include "../../core/state/Debug.h"
#include "../../systems/inventory/InventoryCombat.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/diary/ScreenDiary.h"

bool MapInputControllera::processarInputEComandos(char tecla, Character* jogador, int& proximaPosicaoX, int& proximaPosicaoY, const std::function<void()>& restaurarTela)
{
    if (tecla == 27) { 
        TelaPause::display(jogador); 
        restaurarTela(); 
        return true; 
    }
    if (Debug::isDebugKey(tecla)) { 
        Debug::displayDebugMenu(jogador); 
        restaurarTela(); 
        return true; 
    }

    ComandoMapa comando = InputControl::traduzirTeclaParaComando(tecla);

    if (comando == ComandoMapa::Cima) { proximaPosicaoY--; return false; }
    if (comando == ComandoMapa::Baixo) { proximaPosicaoY++; return false; }
    if (comando == ComandoMapa::Esquerda) { proximaPosicaoX--; return false; }
    if (comando == ComandoMapa::Direita) { proximaPosicaoX++; return false; }

    if (comando == ComandoMapa::Inventory)
    {
        InventarioCombate::gerenciarInventario(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Ficha)
    {
        TelaAtributos::gerenciarFichaDoJogador(jogador);
        restaurarTela();
        return true;
    }
    if (comando == ComandoMapa::Bestiary)
    {
        TelaDiario::display(jogador);
        restaurarTela();
        return true;
    }
    return false;
}
