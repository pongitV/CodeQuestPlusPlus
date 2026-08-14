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

bool MapInputController::processInputAndCommands(char key, Character* player, int& nextX, int& nextY, const std::function<void()>& restoreScreen)
{
    if (key == 27) { 
        TelaPause::display(player); 
        restoreScreen(); 
        return true; 
    }
    if (Debug::isDebugKey(key)) { 
        Debug::displayDebugMenu(player); 
        restoreScreen(); 
        return true; 
    }

    MapCommand cmd = InputControl::translateKeyToCommand(key);

    if (cmd == MapCommand::Up) { nextY--; return false; }
    if (cmd == MapCommand::Down) { nextY++; return false; }
    if (cmd == MapCommand::Left) { nextX--; return false; }
    if (cmd == MapCommand::Right) { nextX++; return false; }

    if (cmd == MapCommand::Inventory)
    {
        InventarioCombate::gerenciarInventario(player);
        restoreScreen();
        return true;
    }
    if (cmd == MapCommand::CharacterSheet)
    {
        TelaAtributos::gerenciarFichaDoJogador(player);
        restoreScreen();
        return true;
    }
    if (cmd == MapCommand::Bestiary)
    {
        TelaDiario::display(player);
        restoreScreen();
        return true;
    }
    return false;
}
