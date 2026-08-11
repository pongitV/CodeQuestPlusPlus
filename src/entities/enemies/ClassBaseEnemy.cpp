#include "ClassBaseEnemy.h"

// --- INFORMACOES DA CLASSE ---
std::string ClassBaseInimigo::getNameClasse() const 
{ 
    return "Monstro"; 
}

Attributes ClassBaseInimigo::obterAtributosClasse() const 
{ 
    return { 0, 0, 0, 0, 0, 0, 0 }; 
}

const std::vector<std::string>& ClassBaseInimigo::obterAparenciaClasseMenu() const 
{ 
    static const std::vector<std::string> aparenciaVazia = {};
    return aparenciaVazia;
}

std::vector<std::unique_ptr<Item>> ClassBaseInimigo::obterEquipamentoClasse() const 
{ 
    return {}; 
}

// --- PASSIVA DA CLASSE ---
std::string ClassBaseInimigo::getNamePassivaClasse() const { return "Nenhuma"; }
std::string ClassBaseInimigo::obterDescricaoPassivaClasse() const { return "Enemies nao possuem passivas de classe."; }

// --- HABILIDADE DA CLASSE ---
std::string ClassBaseInimigo::obterRecargaHabilidadeClasse() const { return ""; }
std::string ClassBaseInimigo::getNameHabilidadeClasse() const { return "Nenhuma"; }
std::string ClassBaseInimigo::getClassAbilityDescription() const { return "Enemies basicos nao possuem habilidades ativas."; }
void ClassBaseInimigo::useClassAbility(Combat* /*combat*/, Character* /*personagemUsuario*/, std::vector<Character*>& /*listaDeInimigos*/) 
{
}

TipoAtaque ClassBaseInimigo::getAttackType() const { return TipoAtaque::UNICO; }
bool ClassBaseInimigo::abilityConsumesTurn() const { return true; }
