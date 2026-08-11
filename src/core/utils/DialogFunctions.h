#pragma once

#include <string>
#include <vector>
#include "../../core/utils/Color.h"
class DialogFunctions {
public:
    static void printNPCDialog(const std::string& npcNome, Color npcCor, const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true);
    static void printNPCDialog(const std::string& npcNome, Color npcCor, const std::vector<std::string>& linhas);
    
    static std::string formatarMsgNarracao(const std::string& texto);
    static std::string formatarMsgSistema(const std::string& texto, Color corTema = Color::YELLOW);
    static std::string formatarMsgHabilidade(const std::string& texto, Color corTema = Color::GREEN_CLARO);
    static std::string formatarMsgStatus(const std::string& texto, Color corTema = Color::YELLOW);
    static std::string formatarMsgDrop(const std::string& texto, Color corTema = Color::WHITE);
    static std::string formatarMsgCombate(const std::string& texto, Color corTema = Color::WHITE);
    static std::string formatarMsgInteracao(const std::string& texto, Color corTema = Color::CYAN);
};