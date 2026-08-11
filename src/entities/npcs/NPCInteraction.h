#pragma once

#include <string>
#include <vector>
#include "../../systems/inventory/Item.h"
#include "../../core/utils/Color.h"

class Character;

class NPCInteraction {
public:
    virtual ~NPCInteraction() = default;

    virtual std::string getNameDoLugar() const = 0;
    virtual Color obterCorDoCabecalho() const = 0;
    virtual std::vector<std::string> obterDialogo(Character* currentPlayer) = 0;
    virtual std::vector<std::string> obterOpcoesMenu(Character* currentPlayer, int larguraDoTerminal) = 0;
    virtual void processarOpcao(Character* currentPlayer, const std::string& opcao, int larguraDoTerminal) = 0;
    virtual Color obterCorDaArte() const = 0;
    virtual const std::vector<std::string>& obterArteASCII() const = 0;

    void interagir(Character* currentPlayer);

    static void processarMenuMissoesVazio(Character* currentPlayer, const std::string& tituloMenu, Color corCabecalho, const std::string& nomeNPC, const std::string& falaVazia);
    static bool verificarMaterialNoInventario(Character* currentPlayer, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Color corNPC, const std::string& mensagemPersonalizada = "");
    static bool verificarItemNaoEquipado(Character* currentPlayer, Item* itemAvaliado, const std::string& nomeNPC, Color corNPC, const std::string& msgErro);
    static Item* lerItemDoInventario(Character* currentPlayer, const std::string& mensagemDialogo, const std::string& nomeNPC, Color corNPC, std::string& codigoSaida, bool displayPrecos = false);
    static void displayTelaDeSucesso(const std::string& tituloCabecalho, Color corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC);
    static std::string obterFormatadorStatusItem(ItemID id);
};
