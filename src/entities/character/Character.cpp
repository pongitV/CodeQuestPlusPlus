#include "Character.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <cassert>

#include "../classes/ClassBase.h"
#include "../races/RaceBase.h"
#include "../classes/necro-clone/NecroClone.h"
#include "../../core/utils/Constants.h"
#include "../../ui/screens/combat/ScreenCombat.h"
std::unordered_set<Character*> Character::personagensAtivos;

bool Character::isValido(Character* p) {
    return personagensAtivos.find(p) != personagensAtivos.end();
}

Character::Character(const Character& other)
    : nomePersonagem(other.nomePersonagem),
      vidaAtual(other.vidaAtual),
      race(std::make_unique<RaceClone>(other.race ? other.race->getRaceName() : "Desconhecido", other.race ? other.race->getRaceAppearance() : std::vector<std::string>())),
      classe(std::make_unique<PlayerClassClone>()),
      statsFinais(other.statsFinais),
      mochila(std::make_unique<Inventory>()),
      itemSelecionadoParaUso(nullptr),
      sistemaDeNivel(std::make_unique<SistemaDeNivel>(other.sistemaDeNivel->getLevel(), other.sistemaDeNivel->getXpAtual(), other.sistemaDeNivel->getXpParaSubir()))
{
    system = other.system;
    
    combat.estaDefendendo = other.combat.estaDefendendo;
    // almasColetadas nao sao copiadas
    combat.recargaDefesa = other.combat.recargaDefesa;
    combat.recargaHabilidade = other.combat.recargaHabilidade;
    combat.pularTurnoInimigo = other.combat.pularTurnoInimigo;
    combat.habilidadeCancelada = other.combat.habilidadeCancelada;
    combat.morteAnimada = other.combat.morteAnimada;
    combat.multiplicadorAtual = other.combat.multiplicadorAtual;
    combat.totalHealingReceived = other.combat.totalHealingReceived;
    combat.vidaMaximaFixa = other.combat.vidaMaximaFixa;
    combat.cooldownsAtivos = other.combat.cooldownsAtivos;

    cache_ = other.cache_;
    personagensAtivos.insert(this);

    // Copia dos Itens (Conforme regra: "mas possui os mesmos items")
    for (const auto& par : other.equipamentos) {
        if (par.second) {
            auto copiaItem = ItemFactory::criarItem(par.second->getNameItem());
            if (copiaItem) { 
                this->equipamentos[par.first] = copiaItem.get(); 
                this->mochila->adicionarItem(std::move(copiaItem)); 
            }
        }
    }
    atualizarCacheSeNecessario();
}

Character::Character(const std::string& nome, std::unique_ptr<RaceBase> racaEscolhida, std::unique_ptr<ClassBase> classeEscolhida)
    : nomePersonagem(nome),
      vidaAtual(0),
      race(std::move(racaEscolhida)),
      classe(std::move(classeEscolhida)),
      statsFinais{ 0, 0, 0, 0, 0, 0, 0 },
      mochila(std::make_unique<Inventory>()),
      itemSelecionadoParaUso(nullptr),
      sistemaDeNivel(std::make_unique<SistemaDeNivel>(1, 0, Constants::BASE_XP_TO_LEVEL_UP))
{
    auto receberEEquiparKit = [this](std::vector<std::unique_ptr<Item>> kit) {
        for (auto& itemUnique : kit) {
            Item* ptr = itemUnique.get();
            this->mochila->adicionarItem(std::move(itemUnique)); 
            this->equiparItem(ptr);            
        }
    };

    receberEEquiparKit(this->classe->obterEquipamentoClasse());
    receberEEquiparKit(this->race->getRaceEquipment());

    calcularAtributos();
    personagensAtivos.insert(this);
}

Character::~Character() 
{
    personagensAtivos.erase(this);
}  

std::unique_ptr<Character> Character::clone() const {
    return std::make_unique<Character>(*this);
}

void Character::escalarAtributos(double fator) {
    statsFinais.scaleAll(fator);
    combat.vidaMaximaFixa = statsFinais.health; 
    strengthrRecalculoCache();
    vidaAtual = obterVidaMaxima();
}

void Character::adicionarAlma(std::unique_ptr<Character> alma) { combat.almasColetadas.push_back(std::move(alma)); }

std::vector<std::unique_ptr<Character>>& Character::obterAlmas() { return combat.almasColetadas; }

size_t Character::obterNumeroDeAlmas() const { return combat.almasColetadas.size(); }

std::unique_ptr<Character> Character::removerAlma(int index) {
    if (index < 0 || index >= static_cast<int>(combat.almasColetadas.size())) return nullptr;
    auto alma = std::move(combat.almasColetadas[index]);
    combat.almasColetadas.erase(combat.almasColetadas.begin() + index);
    return alma;
}

int* Character::obterPonteiroAtributoEstatico(TipoAtributo atributo) {
    switch (atributo) {
        case TipoAtributo::Forca: return &statsFinais.strength;
        case TipoAtributo::Destreza: return &statsFinais.dexterity;
        case TipoAtributo::Resistencia: return &statsFinais.resistance;
        case TipoAtributo::Constituicao: return &statsFinais.constitution;
        case TipoAtributo::Inteligencia: return &statsFinais.intelligence;
        case TipoAtributo::Sabedoria: return &statsFinais.wisdom;
        default: return nullptr;
    }
}

bool Character::subirDeNivel(TipoAtributo atributo)
{
    if (sistemaDeNivel->getXpAtual() < sistemaDeNivel->getXpParaSubir()) return false;

    if (atributo == TipoAtributo::Health) {
        statsFinais.health += Constants::HEALTH_GAIN_PER_LEVEL;
        vidaAtual += Constants::HEALTH_GAIN_PER_LEVEL;
    } else if (int* attr = obterPonteiroAtributoEstatico(atributo)) {
        *attr += Constants::STAT_GAIN_PER_LEVEL;
    } else {
        return false;
    }

    sistemaDeNivel->definirXpAtual(sistemaDeNivel->getXpAtual() - sistemaDeNivel->getXpParaSubir());
    sistemaDeNivel->definirXpParaSubir(static_cast<int>(std::min(sistemaDeNivel->getXpParaSubir() * Constants::XP_MULTIPLIER_PER_LEVEL, Constants::MAX_XP)));
    sistemaDeNivel->definirNivel(sistemaDeNivel->getLevel() + 1);
    cache_.sujo = true;
    return true;
}

void Character::alterarAtributoEstatico(TipoAtributo atributo, int valor)
{
    if (int* attr = obterPonteiroAtributoEstatico(atributo)) {
        *attr = std::max(0, *attr + valor);
        cache_.sujo = true;
    }
}

void Character::reduzirCooldowns()
{
    if (combat.recargaDefesa) combat.recargaDefesa = false;
    if (combat.recargaHabilidade) combat.recargaHabilidade = false;
    if (combat.cooldownsAtivos.empty()) return;
    for (auto& par : combat.cooldownsAtivos)
    {
        if (par.second > 0) par.second--;
    }
}

void Character::prepararParaNovaBatalha()
{
    combat.resetar();
    combat.vidaMaximaFixa = obterVidaMaxima();
    limparEfeitos();
    
    if (obterArmadura() && obterArmadura()->temPropriedade(Propriedade::ArmaduraAdaptacao)) {
        adicionarEfeito(std::make_unique<EfeitoRodaAdaptacao>());
    }
}

void Character::calcularAtributos()
{
    this->statsFinais.addAttributes(race->getRaceAttributes());
    this->statsFinais.addAttributes(classe->obterAtributosClasse());
    this->vidaAtual = obterVidaMaxima();
    cache_.sujo = true;
}

void Character::atualizarCacheSeNecessario() const {
    std::lock_guard<std::mutex> lock(mutexCache_);
    if (!cache_.sujo) return;
    
    double mult = system.difficultyMultiplicador;
    auto aplicarMult = [mult](int val) { return static_cast<int>(val * mult); };

    cache_.vidaMaxima = aplicarMult(statsFinais.health);
    cache_.strength = aplicarMult(statsFinais.strength);
    cache_.resistance = aplicarMult(statsFinais.resistance);
    cache_.constitution = aplicarMult(statsFinais.constitution);
    cache_.intelligence = aplicarMult(statsFinais.intelligence);
    cache_.wisdom = aplicarMult(statsFinais.wisdom);

    int penalidade = obterArmadura() ? (obterArmadura()->obterReducaoFixa() / 3) : 0;
    if (obterArmadura() && obterArmadura()->getNameItem() == "Armor de bau") penalidade = 10;
    if (classe) penalidade = classe->processarPenalidadeArmaduraPassivaArqueiro(penalidade);
    
    int dexterityBase = static_cast<int>(statsFinais.dexterity * mult);
    int dexterityFinal = dexterityBase - penalidade;
    cache_.dexterity = dexterityFinal > 0 ? dexterityFinal : 0;

    int bonusArmadura = obterArmadura() ? obterArmadura()->obterReducaoFixa() : 0;
    int reducao = cache_.resistance + bonusArmadura;
    
    double percentualReducao = cache_.constitution / 100.0;
    if (percentualReducao > 0.50) percentualReducao = 0.50;
    cache_.reducaoPercentual = static_cast<int>(reducao * (1.0 - percentualReducao));

    cache_.sujo = false;
}

void Character::definirMultiplicador(double novoMultiplicador) 
{ 
    if (classe) {
        combat.multiplicadorAtual = classe->processarMultiplicadorBuffPassivaBard(novoMultiplicador);
    } else {
        combat.multiplicadorAtual = novoMultiplicador;
    }
}

void Character::aplicarMultiplicadorDificuldade(double mult)
{
    if (mult <= 1.0) return;
    system.difficultyMultiplicador = mult;
    cache_.sujo = true;
    this->vidaAtual = obterVidaMaxima();
}

void Character::modificarVida(int valor) 
{
    if (valor > 0 && classe != nullptr) {
        valor = classe->processarCuraPassivaBard(valor);
    }

    int vidaAntes = this->vidaAtual;
    this->vidaAtual = std::clamp(this->vidaAtual + valor, 0, obterVidaMaxima());

    if (this->vidaAtual > vidaAntes) 
    {
        combat.totalHealingReceived += (this->vidaAtual - vidaAntes);
    }
}

const EfeitoStatus* Character::encontrarEfeito(EfeitoID id) const {
    auto it = std::find_if(efeitosAtivos.begin(), efeitosAtivos.end(), [id](const auto& ef) {
        return ef->obterID() == id;
    });
    return it != efeitosAtivos.end() ? it->get() : nullptr;
}

bool Character::possuiEfeito(EfeitoID id) const {
    return encontrarEfeito(id) != nullptr;
}

int Character::obterTurnosEfeito(EfeitoID id) const {
    const EfeitoStatus* ef = encontrarEfeito(id);
    return ef ? ef->obterTurnosRestantes() : 0;
}

void Character::mostrarStatus() const 
{
}

std::string Character::getNameClasse() const 
{
    return this->classe->getNameClasse();
}

ClassType Character::obterClassType() const 
{
    if (this->classe) return this->classe->obterClassType();
    return ClassType::Nenhum;
}

RaceType Character::obterRaceType() const 
{
    if (this->race) return this->race->obterRaceType();
    return RaceType::Nenhum;
}

void Character::equiparItem(Item* item)
{
    if (item == nullptr) return;
    if (item->obterTipo() == TipoEquipamento::ARMA) this->equipamentos[EquipmentSlot::MAO_PRINCIPAL] = item;
    else if (item->obterTipo() == TipoEquipamento::ESCUDO) this->equipamentos[EquipmentSlot::MAO_SECUNDARIA] = item;
    else if (item->obterTipo() == TipoEquipamento::ARMADURA)
    {
        this->equipamentos[EquipmentSlot::ARMADURA] = item;
        if (combat.vidaMaximaFixa > 0 && item->temPropriedade(Propriedade::ArmaduraAdaptacao)) {
            if (!possuiEfeito(EfeitoID::RodaAdaptacao)) {
                adicionarEfeito(std::make_unique<EfeitoRodaAdaptacao>());
            }
        }
    }
    else if (item->obterTipo() == TipoEquipamento::CONSUMIVEL) this->equipamentos[EquipmentSlot::CONSUMIVEL] = item;
    cache_.sujo = true;
}

RaceBase* Character::obterRaca() const 
{
    return this->race.get();
}

ClassBase* Character::obterClasse() const 
{
    return this->classe.get();
}

TipoAtaque Character::getAttackType() const 
{
    if (this->classe) return this->classe->getAttackType();
    return TipoAtaque::UNICO;
}

bool Character::habilidadeDaClasseConsomeTurno() const 
{
    if (this->classe) return this->classe->abilityConsumesTurn();
    return true;
}

int Character::calcularDefesaBase(int danoBruto, int danoPerfurante) {
    int danoSemPerfuracao = std::max(0, danoBruto - danoPerfurante);

    atualizarCacheSeNecessario();

    int finalDamage = static_cast<int>(danoSemPerfuracao - cache_.reducaoPercentual);
    if (finalDamage < 1 && danoSemPerfuracao > 0) finalDamage = 1;
    else if (danoSemPerfuracao == 0) finalDamage = 0;

    return finalDamage + danoPerfurante;
}

ResultadoDano Character::receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas) {
    ResultadoDano resultado;

    int finalDamage = calcularDefesaBase(danoBruto, danoPerfurante);

    for (auto& ef : efeitosAtivos) {
        finalDamage = ef->processarDanoRecebido(finalDamage);
    }

    finalDamage = std::max(0, finalDamage - danoReduzidoParry);

    if (combat.estaDefendendo && obterEscudo() != nullptr) {
        Item* esc = obterEscudo();
        resultado.danoBloqueado = esc->obterReducaoDanoFixaEscudo();
        finalDamage = std::max(0, finalDamage - resultado.danoBloqueado);

        esc->reduzirDurabilidade(1);
        if (esc->obterDurabilidadeAtualEscudo() <= 0) {
            resultado.escudoQuebrou = true;
            resultado.nomeEscudoQuebrado = esc->getNameItem();
            mochila->removerItem(esc);
            desequiparEscudo();
        }
    }

    if (aplicarPassivas && race) finalDamage = race->processDefensiveDamage(finalDamage, this);
    
    if (atacante) finalDamage = atacante->garantirDanoMinimo(finalDamage);

    if (finalDamage > 0) modificarVida(-finalDamage);

    resultado.finalDamage = finalDamage;
    return resultado;
}

void Character::adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito) {
    efeito->aoEntrar(this);
    if (processandoEfeitos) {
        efeitosFilaAdicao.push_back(std::move(efeito));
    } else {
        efeitosAtivos.push_back(std::move(efeito));
    }
    cache_.sujo = true;
}

void Character::processarEfeitosInicioTurno() {
    processandoEfeitos = true;
    for (auto& ef : efeitosAtivos) {
        ef->aplicarInicioTurno(this);
        ef->decrementarTurno();
    }

    efeitosAtivos.erase(
        std::remove_if(efeitosAtivos.begin(), efeitosAtivos.end(),
            [this](const std::unique_ptr<EfeitoStatus>& ef) {
                if (ef->expirou()) {
                    ef->aoSair(this);
                    cache_.sujo = true;
                    return true;
                }
                return false;
            }),
        efeitosAtivos.end()
    );
    processandoEfeitos = false;

    for (EfeitoID id : efeitosFilaRemocao) {
        removerEfeito(id);
    }
    efeitosFilaRemocao.clear();

    for (auto& ef : efeitosFilaAdicao) {
        efeitosAtivos.push_back(std::move(ef));
    }
    efeitosFilaAdicao.clear();
}

void Character::limparEfeitos() {
    for (auto& ef : efeitosAtivos) {
        ef->aoSair(this); // Garante que os attributes (como Forca e Destreza) sejam restaurados
    }
    efeitosAtivos.clear();
    efeitosFilaAdicao.clear();
    efeitosFilaRemocao.clear();
    cache_.sujo = true;
}

void Character::removerEfeito(EfeitoID id) {
    if (processandoEfeitos) {
        efeitosFilaRemocao.push_back(id);
        return;
    }
    auto it = std::find_if(efeitosAtivos.begin(), efeitosAtivos.end(),
        [id](const std::unique_ptr<EfeitoStatus>& ef) {
            return ef->obterID() == id;
        });
    if (it != efeitosAtivos.end()) {
        (*it)->aoSair(this);
        efeitosAtivos.erase(it);
        cache_.sujo = true;
    }
}

bool Character::podeAgir(std::string& outMotivoIncapacidade) const {
    auto it = std::find_if(efeitosAtivos.begin(), efeitosAtivos.end(), [](const auto& ef) {
        return ef->impedeAcao();
    });
    if (it != efeitosAtivos.end()) {
        outMotivoIncapacidade = (*it)->getName();
        return false;
    }
    return true;
}

void Character::obterIDsEfeitosAtivos(std::vector<EfeitoID>& outIDs) const {
    outIDs.clear();
    outIDs.reserve(efeitosAtivos.size());
    std::transform(efeitosAtivos.begin(), efeitosAtivos.end(), std::back_inserter(outIDs), [](const auto& ef) {
        return ef->obterID();
    });
}

void Character::executarDrops(Character* currentPlayer, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) {
    if (race) {
        race->realizarDrops(this, currentPlayer, itensObtidos, ouroTotal, xpTotal);
    }
}

int Character::garantirDanoMinimo(int danoAtual) {
    if (obterArma()) {
        return obterArma()->garantirDanoMinimo(danoAtual);
    }
    return danoAtual;
}

std::pair<int, int> Character::calcularDanoOfensivoBase() {
    double multiplicadorDeAtributos = obterMultiplicador();

    int danoFisicoDaArma = 1;
    int danoMagicoDaArma = 0;
    int perfuranteAtual = 0;

    if (obterArma()) 
    {
        danoFisicoDaArma = obterArma()->obterDanoFisico();
        danoMagicoDaArma = obterArma()->obterDanoMagico();

        if (obterArma()->temPropriedade(Propriedade::Magica)) {
            int bonusMagico = danoFisicoDaArma / 2;
            double bonusEscalado = bonusMagico * (1.0 + (getWisdom() / 100.0));
            perfuranteAtual = static_cast<int>(bonusEscalado * multiplicadorDeAtributos);
        }
    }

    int strengthEfetiva = getStrength();
    int dexterityEfetiva = getDexterity();
    int intelligenceEfetiva = getInteligencia();
    int wisdomEfetiva = getWisdom();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        strengthEfetiva /= 10; dexterityEfetiva /= 10;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        intelligenceEfetiva /= 10; wisdomEfetiva /= 10;
    }

    int danoFisicoCalculado = std::max(0, static_cast<int>((danoFisicoDaArma + strengthEfetiva) * (1.0 + (dexterityEfetiva / 100.0))));
    int danoMagicoCalculado = std::max(0, static_cast<int>((danoMagicoDaArma + intelligenceEfetiva) * (1.0 + (wisdomEfetiva / 100.0))));
    
    int total = std::max(1, danoFisicoCalculado + danoMagicoCalculado);
    int totalFinal = static_cast<int>(total * multiplicadorDeAtributos);
    int perfuranteFinal = perfuranteAtual;

    if (obterArma() && obterArma()->temPropriedade(Propriedade::IgnoraDefesa)) {
        perfuranteFinal = totalFinal;
    }

    if (race && race->ignoraEscudo()) {
        perfuranteFinal = totalFinal;
    }

    if (possuiEfeito(EfeitoID::MiraCerteira)) {
        totalFinal *= 2;
        perfuranteFinal *= 2;
        removerEfeito(EfeitoID::MiraCerteira);
    }

    return { totalFinal, perfuranteFinal };
}

void Character::finalizarBatalha() { 
    combat.vidaMaximaFixa = 0; 
    if (system.possuiRegeneracaoTroll && vidaAtual > 0 && vidaAtual < obterVidaMaxima()) {
        modificarVida(obterVidaMaxima());
        InputControl::aguardarEnter();
    }
}

bool Character::isBoss() const {
    RaceType t = obterRaceType();
    return (t == RaceType::Mahoraga || 
            t == RaceType::OrkExilado || 
            t == RaceType::Troll || 
            t == RaceType::AbominacaoFloresta);
}
