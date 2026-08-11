#include "DialogFunctions.h"
#include "StringBuffer.h"
#include <iostream>
#include "../../core/utils/Color.h"

void DialogFunctions::printNPCDialog(const std::string& npcNome, Color /*npcCor*/, const std::string& texto, bool novaLinhaAntes, bool /*novaLinhaDepois*/) {
    if (texto.empty()) return;
    StringBuffer buf(3);
    buf.Append(novaLinhaAntes ? "\n[" : "  [");
    buf.Append(npcNome);
    buf.Append("]: ");
    buf.Append(texto);
}

void DialogFunctions::printNPCDialog(const std::string& npcNome, Color npcCor, const std::vector<std::string>& linhas) {
    if (linhas.empty()) return;
    
    printNPCDialog(npcNome, npcCor, linhas[0], true, true);
    for (size_t i = 1; i < linhas.size(); ++i) {
        printNPCDialog(npcNome, npcCor, linhas[i], false, true);
    }
}

std::string DialogFunctions::formatarMsgNarracao(const std::string& texto) {
    StringBuffer buf(2);
    buf.Append("[NARRACAO]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatarMsgSistema(const std::string& texto, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[SISTEMA]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatarMsgHabilidade(const std::string& texto, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[HABILIDADE]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatarMsgStatus(const std::string& texto, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[STATUS]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatarMsgDrop(const std::string& texto, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[DROP]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatarMsgCombate(const std::string& texto, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[COMBATE]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}

std::string DialogFunctions::formatarMsgInteracao(const std::string& texto, Color /*cor*/) {
    StringBuffer buf(2);
    buf.Append("[INTERACAO]: ");
    buf.Append(texto);
    auto vec = buf.ToVector();
    return vec[0] + vec[1];
}
