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
        int selecao = TelaMenu::displayOpcoesMenuPrincipal();
        
        std::string opcaoSelecionada;
        if (selecao == 0) opcaoSelecionada = "Novo Game";
        else opcaoSelecionada = "Sair";
        
        if (opcaoSelecionada == "Novo Game") {
            auto novoJogador = startCharacterCreationSystem();
            if (novoJogador) return novoJogador;
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
    std::string nomeDoPersonagem;
    std::unique_ptr<RaceBase> racaEscolhida;
    std::unique_ptr<ClassBase> classeEscolhida;
    bool sistemaDeParryAtivado = false;
    TelaParry::Resultado::Modo modoParry = TelaParry::Resultado::Modo::Desligado;
    int nivelDeDificuldadeEscolhido = 2;

    for (;;) {
        {
            auto resultado = TelaNome::display();
            if (resultado.voltou) return nullptr;
            nomeDoPersonagem = resultado.nome;
        }

        std::string nomeRaca;
        {
            auto resultado = TelaRaca::display(nomeDoPersonagem);
            if (resultado.voltou) continue;

            nomeRaca = resultado.nome;
            racaEscolhida = FabricaRacas::createRace(resultado.racaSelecionada);
        }

        std::string nomeClasse;
        {
            auto resultado = TelaClasse::display(nomeDoPersonagem, nomeRaca);
            if (resultado.voltou) continue;

            nomeClasse = resultado.nome;
            classeEscolhida = ClassFactory::createClass(resultado.classeSelecionada);
        }

        {
            auto resultado = TelaDificuldade::display(nomeDoPersonagem, nomeRaca, nomeClasse);
            if (resultado.voltou) continue;
            nivelDeDificuldadeEscolhido = resultado.indice + 1;
        }

        {
            auto resultado = TelaParry::display(nomeDoPersonagem, nomeRaca, nomeClasse);
            if (resultado.voltou) continue;
            modoParry = resultado.modo;
            sistemaDeParryAtivado = resultado.modo != TelaParry::Resultado::Modo::Desligado;
        }

        auto personagemCriado = std::make_unique<Character>(nomeDoPersonagem, std::move(racaEscolhida), std::move(classeEscolhida));
        personagemCriado->definirParryAtivado(sistemaDeParryAtivado);
        personagemCriado->definirParryModerno(modoParry == TelaParry::Resultado::Modo::Movimento);
        personagemCriado->definirDificuldade(static_cast<DificuldadeJogo>(nivelDeDificuldadeEscolhido));

        Diary::instancia().registrarRaca(personagemCriado->obterRaca()->getRaceName());
        Diary::instancia().registrarClasse(personagemCriado->getNameClasse());

        for (Item* item : personagemCriado->obterInventario()->obterTodosOsItens()) {
            Diary::instancia().registrarItem(item->getNameItem());
        }

        TelaIntroducao::display();
        return personagemCriado;
    }
}
