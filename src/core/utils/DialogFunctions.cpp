#include "DialogFunctions.h"
#include "StringBuffer.h"
#include <iostream>
#include "../../core/utils/Color.h"

void DialogFunctions::printNPCDialog(const std::string& npcName, Color /*npcColor*/, const std::string& text, bool newlineBefore, bool /*newlineAfter*/) {
    if (text.empty()) return;
    StringBuffer buf(3);
    buf.Append(newlineBefore ? "\n[" : "  [");
    buf.Append(npcName);
    buf.Append("]: ");
    buf.Append(text);
}

void DialogFunctions::printNPCDialog(const std::string& npcName, Color npcColor, const std::vector<std::string>& lines) {
    if (lines.empty()) return;
    
    printNPCDialog(npcName, npcColor, lines[0], true, true);
    for (size_t i = 1; i < lines.size(); ++i) {
        printNPCDialog(npcName, npcColor, lines[i], false, true);
    }
}

std::string DialogFunctions::formatNarrationMsg(const std::string& text) {
    StringBuffer buf(2);
    buf.Append("[NARRACAO]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatSystemMsg(const std::string& text, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[SISTEMA]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatAbilityMsg(const std::string& text, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[HABILIDADE]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatStatusMsg(const std::string& text, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[STATUS]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatDropMsg(const std::string& text, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[DROP]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatCombatMsg(const std::string& text, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[COMBATE]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatInteractionMsg(const std::string& text, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[INTERACAO]: ");
    buf.Append(text);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}
