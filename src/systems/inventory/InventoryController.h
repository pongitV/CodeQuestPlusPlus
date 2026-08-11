#pragma once
#include <string>

class Character;
class Item;

enum class ResultadoItem {
    Equipou,
    Desequipou,
    Usou_Turno,
    Usou_SemTurno,
    Erro_TurnoJaUsado,
    Erro_EscudoQuebrado,
    Erro_Requisitos,
    Erro_NaoPodeUsar,
    Nada
};

struct UsoItemInfo {
    ResultadoItem resultado;
    std::string nomeItem;
    std::string mensagemExtra;
    bool consumiuTurno = false;
};

class ControleInventario {
public:
    static UsoItemInfo usarOuEquipar(Character* jogador, Item* item, bool turnoJaFoiConsumido);
    static std::string obterMensagemErro(Item* item, bool emCombate);
};
