#include "GameMenu.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "../../entities/classes/ClassFactory.h"
#include "../../entities/races/RaceFactory.h"
#include "EnemyCreator.h"
#include "../../systems/inventory/equipment/WeaponEquipment.h"
#include "../../systems/inventory/equipment/ArmorEquipment.h"
#include "../../systems/inventory/equipment/ShieldEquipment.h"
#include "../../systems/inventory/Item.h"
#include "../../systems/inventory/items/ConsumableItem.h"
#include "../../entities/races/dwarf/Dwarf.h"
#include "../../entities/races/elf/Elf.h"
#include "../../entities/races/human/Human.h"
#include "../../entities/races/orc/Orc.h"
#include "../../entities/races/RaceBase.h"
#include "../../entities/enemies/goblin/Goblin.h"
#include "../../entities/enemies/slime/Slime.h"
#include "../../entities/enemies/troll/Troll.h"
#include "../../entities/enemies/fairy/Fairy.h"
#include "../../entities/enemies/mimic/Mimic.h"
#include "../../systems/progress/Diary.h"
#include "../../ui/screens/attributes/ScreenAttributes.h"
#include "../../ui/screens/inventory/ScreenInventory.h"
#include "../../ui/screens/menu/ScreenMenu.h"
#include "../../ui/screens/menu/ScreenOpening.h"
#include "../../ui/screens/menu/ScreenName.h"
#include "../../ui/screens/menu/ScreenRace.h"
#include "../../ui/screens/menu/ScreenClass.h"
#include "../../ui/screens/menu/ScreenDifficulty.h"
#include "../../ui/screens/menu/ScreenParry.h"
#include "../../ui/screens/menu/ScreenIntroduction.h"
#include "../../ui/screens/utils/ScreenRegistry.h"
#include "../utils/DialogFunctions.h"
#include "../utils/InputControl.h"
#include "../utils/RandomGenerator.h"
#include "../../ui/screens/ScreenBase.h"

std::unique_ptr<Character> GameMenu::mainMenu() 
{
    TelaAbertura::display();

    while (true) {
        int selection = TelaMenu::displayOpcoesMenuPrincipal();
        
        std::string selectedOption;
        if (selection == 0) selectedOption = "Novo Game";
        else selectedOption = "Sair";
        
        if (selectedOption == "Novo Game") {
            auto newPlayer = startCharacterCreationSystem();
            if (newPlayer) return newPlayer;
        } else {
            if (RegistroTelas::modoRaycasterAtivo()) {
                return nullptr;
            } else {
                if (RegistroTelas::confirmarSaida()) {
                    return nullptr;
                }
            }
        }
    }
}

std::unique_ptr<Character> GameMenu::startCharacterCreationSystem() 
{
    std::string characterName;
    std::unique_ptr<RaceBase> chosenRace;
    std::unique_ptr<ClassBase> chosenClass;
    bool parrySystemEnabled = false;
    TelaParry::Resultado::Modo parryMode = TelaParry::Resultado::Modo::Desligado;
    int chosenDifficultyLevel = 2;

    for (;;) {
        {
            auto result = TelaNome::display();
            if (result.voltou) return nullptr;
            characterName = result.nome;
        }

        std::string raceName;
        {
            auto result = TelaRaca::display(characterName);
            if (result.voltou) continue;

            raceName = result.nome;
            chosenRace = FabricaRacas::createRace(result.racaSelecionada);
        }

        std::string className;
        {
            auto result = TelaClasse::display(characterName, raceName);
            if (result.voltou) continue;

            className = result.nome;
            chosenClass = ClassFactory::createClass(result.classeSelecionada);
        }

        {
            auto result = TelaDificuldade::display(characterName, raceName, className);
            if (result.voltou) continue;
            chosenDifficultyLevel = result.indice + 1;
        }

        {
            auto result = TelaParry::display(characterName, raceName, className);
            if (result.voltou) continue;
            parryMode = result.modo;
            parrySystemEnabled = result.modo != TelaParry::Resultado::Modo::Desligado;
        }

        auto createdCharacter = std::make_unique<Character>(characterName, std::move(chosenRace), std::move(chosenClass));
        createdCharacter->definirParryAtivado(parrySystemEnabled);
        createdCharacter->definirParryModerno(parryMode == TelaParry::Resultado::Modo::Movimento);
        createdCharacter->definirDificuldade(static_cast<DificuldadeJogo>(chosenDifficultyLevel));

        Diary::instance().registerRace(createdCharacter->obterRaca()->getRaceName());
        Diary::instance().registerClass(createdCharacter->getNameClasse());

        for (Item* item : createdCharacter->obterInventario()->obterTodosOsItens()) {
            Diary::instance().registerItem(item->getNameItem());
        }

        TelaIntroducao::display();
        return createdCharacter;
    }
}
