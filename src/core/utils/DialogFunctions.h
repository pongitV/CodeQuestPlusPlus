#pragma once

#include <string>
#include <vector>
#include "../../core/utils/Color.h"

class DialogFunctions {
public:
    static void printNPCDialog(const std::string& npcName, Color npcColor, const std::string& text, bool newlineBefore = true, bool newlineAfter = true);
    static void printNPCDialog(const std::string& npcName, Color npcColor, const std::vector<std::string>& lines);
    
    static std::string formatNarrationMsg(const std::string& text);
    static std::string formatSystemMsg(const std::string& text, Color themeColor = Color::YELLOW);
    static std::string formatAbilityMsg(const std::string& text, Color themeColor = Color::LIGHT_GREEN);
    static std::string formatStatusMsg(const std::string& text, Color themeColor = Color::YELLOW);
    static std::string formatDropMsg(const std::string& text, Color themeColor = Color::WHITE);
    static std::string formatCombatMsg(const std::string& text, Color themeColor = Color::WHITE);
    static std::string formatInteractionMsg(const std::string& text, Color themeColor = Color::CYAN);

    // Métodos legados para compatibilidade
    static std::string formatarMsgNarracao(const std::string& texto) { return formatNarrationMsg(texto); }
    static std::string formatarMsgSistema(const std::string& texto, Color corTema = Color::YELLOW) { return formatSystemMsg(texto, corTema); }
    static std::string formatarMsgHabilidade(const std::string& texto, Color corTema = Color::LIGHT_GREEN) { return formatAbilityMsg(texto, corTema); }
    static std::string formatarMsgStatus(const std::string& texto, Color corTema = Color::YELLOW) { return formatStatusMsg(texto, corTema); }
    static std::string formatarMsgDrop(const std::string& texto, Color corTema = Color::WHITE) { return formatDropMsg(texto, corTema); }
    static std::string formatarMsgCombate(const std::string& texto, Color corTema = Color::WHITE) { return formatCombatMsg(texto, corTema); }
    static std::string formatarMsgInteracao(const std::string& texto, Color corTema = Color::CYAN) { return formatInteractionMsg(texto, corTema); }
};