#pragma once

#include <string>
#include <vector>
#include "../../systems/inventory/Item.h"
#include "../../core/utils/Color.h"

class Character;

class NPCInteraction {
public:
    virtual ~NPCInteraction() = default;

    virtual std::string getPlaceName() const = 0;
    virtual std::string getNameDoLugar() const { return getPlaceName(); }

    virtual Color getHeaderColor() const = 0;
    virtual Color obterCorDoCabecalho() const { return getHeaderColor(); }

    virtual std::vector<std::string> getDialogue(Character* currentPlayer) = 0;
    virtual std::vector<std::string> obterDialogo(Character* currentPlayer) { return getDialogue(currentPlayer); }

    virtual std::vector<std::string> getMenuOptions(Character* currentPlayer, int terminalWidth) = 0;
    virtual std::vector<std::string> obterOpcoesMenu(Character* currentPlayer, int terminalWidth) { return getMenuOptions(currentPlayer, terminalWidth); }

    virtual void processOption(Character* currentPlayer, const std::string& option, int terminalWidth) = 0;
    virtual void processarOpcao(Character* currentPlayer, const std::string& option, int terminalWidth) { processOption(currentPlayer, option, terminalWidth); }

    virtual Color getArtColor() const = 0;
    virtual Color obterCorDaArte() const { return getArtColor(); }

    virtual const std::vector<std::string>& getASCIIArt() const = 0;
    virtual const std::vector<std::string>& obterArteASCII() const { return getASCIIArt(); }

    void interact(Character* currentPlayer);
    void interagir(Character* currentPlayer) { interact(currentPlayer); }

    static void processEmptyQuestsMenu(Character* currentPlayer, const std::string& menuTitle, Color headerColor, const std::string& npcName, const std::string& emptyDialogue);
    static void processarMenuMissoesVazio(Character* currentPlayer, const std::string& tituloMenu, Color corCabecalho, const std::string& nomeNPC, const std::string& falaVazia) {
        processEmptyQuestsMenu(currentPlayer, tituloMenu, corCabecalho, nomeNPC, falaVazia);
    }

    static bool verifyMaterialInInventory(Character* currentPlayer, const std::string& materialName, int requiredAmount, const std::string& npcName, Color npcColor, const std::string& customMessage = "");
    static bool verificarMaterialNoInventario(Character* currentPlayer, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Color corNPC, const std::string& mensagemPersonalizada = "") {
        return verifyMaterialInInventory(currentPlayer, nomeMaterial, quantidadeNecessaria, nomeNPC, corNPC, mensagemPersonalizada);
    }

    static bool verifyItemNotEquipped(Character* currentPlayer, Item* evaluatedItem, const std::string& npcName, Color npcColor, const std::string& errorMessage);
    static bool verificarItemNaoEquipado(Character* currentPlayer, Item* itemAvaliado, const std::string& nomeNPC, Color corNPC, const std::string& msgErro) {
        return verifyItemNotEquipped(currentPlayer, itemAvaliado, nomeNPC, corNPC, msgErro);
    }

    static Item* readItemFromInventory(Character* currentPlayer, const std::string& dialogueMessage, const std::string& npcName, Color npcColor, std::string& exitCode, bool displayPrices = false);
    static Item* lerItemDoInventario(Character* currentPlayer, const std::string& mensagemDialogo, const std::string& nomeNPC, Color corNPC, std::string& codigoSaida, bool displayPrecos = false) {
        return readItemFromInventory(currentPlayer, mensagemDialogo, nomeNPC, corNPC, codigoSaida, displayPrecos);
    }

    static void displaySuccessScreen(const std::string& headerTitle, Color headerColor, const std::string& equation, const std::vector<std::string>& asciiArt, const std::string& npcName, const std::string& npcDialogue);
    static void displayTelaDeSucesso(const std::string& tituloCabecalho, Color corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC) {
        displaySuccessScreen(tituloCabecalho, corCabecalho, equacao, arteAscii, nomeNPC, falaNPC);
    }

    static std::string getItemStatusFormatter(ItemID id);
    static std::string obterFormatadorStatusItem(ItemID id) {
        return getItemStatusFormatter(id);
    }
};
