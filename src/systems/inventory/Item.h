#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>

enum class EquipmentType 
{
    None,     
    Weapon,       
    Shield,     
    Armor,   
    Consumable, 
    Quest,
    Material
};

using TipoEquipamento = EquipmentType;

enum class Property 
{
    None,
    Magic,
    Piercing,
    IgnoreDefense,
    BasicGuitar,
    MagicGuitar,
    VineTrap,
    Upgraded,
    UpgradedMaterial,
    HealingConsumable,
    BuffConsumable,
    SlowDebuffConsumable,
    WeaknessDebuffConsumable,
    StrengthTalisman,
    IntelligenceTalisman,
    DexterityTalisman,
    WisdomTalisman,
    TrollPowerConsumable,
    AdaptationArmor
};

using Propriedade = Property;

enum class ItemID {
    None = 0,
    // Armas
    StoneDagger, WoodBow, CrystalStaff, CorrodedWand, EnchantedGuitar, IronSword, BattleAxe, AcidSlimeWeapon, CrumpledTrunk, KnightSword, ExterminationSword, BoneStaff,
    
    // Escudos
    MetalShield, MagicBarrier, MagicCape, SilverBracers,
    
    // Armaduras
    ChainArmor, LeatherArmor, Tunic, NobleOutfit, RagsArmor, KnightArmor, ChestArmor, AdaptationWheel, RitualistClothes,
    
    // Consumíveis
    HealPotion30, FuryPotion, ArcaneElixir, SlimeFlask, WeaknessFlask, RegeneratorOrgan,
    BearTalisman, RavenTalisman, LeopardTalisman, OwlTalisman,
    Apple, Bread, Cheese, DriedMeat,
    LargeHealPotion, AlchemicalStrengthPotion, AlchemicalPoisonPotion, AlchemicalSlowPotion,
    
    // Materiais
    AcidSlime, GoblinTooth, StickyCore, MagicDust, EnchantedWood, ForestHeart, UpgradeStone, RoyalInvitation,
    
    // Missões
    LanguageDevice,

    // Aliases legados
    Nenhum = None,
    AdagaPedra = StoneDagger, ArcoMadeira = WoodBow, CajadoCristal = CrystalStaff, VarinhaCorroida = CorrodedWand, ViolaoEncantado = EnchantedGuitar, EspadaFerro = IronSword, MachadoGuerra = BattleAxe, GosmaAcidaArma = AcidSlimeWeapon, TroncoAmarrotado = CrumpledTrunk, EspadaCavaleiro = KnightSword, EspadaExterminio = ExterminationSword, CajadoOsso = BoneStaff,
    EscudoMetal = MetalShield, BarreiraMagica = MagicBarrier, CapaMagica = MagicCape, BracedeirasPrata = SilverBracers,
    ArmaduraMalha = ChainArmor, ArmaduraCouro = LeatherArmor, Tunica = Tunic, TrajeNobre = NobleOutfit, ArmaduraTrapos = RagsArmor, ArmaduraCavaleiro = KnightArmor, ArmaduraBau = ChestArmor, RodaAdaptacao = AdaptationWheel, RoupasRitualista = RitualistClothes,
    PocaoCura30 = HealPotion30, PocaoFuria = FuryPotion, ElixirArcano = ArcaneElixir, FrascoGosma = SlimeFlask, FrascoFraqueza = WeaknessFlask, OrgaoRegenerador = RegeneratorOrgan,
    TalismaUrso = BearTalisman, TalismaCorvo = RavenTalisman, TalismaLeopardo = LeopardTalisman, TalismaCoruja = OwlTalisman,
    Maca = Apple, Pao = Bread, Queijo = Cheese, CarneSeca = DriedMeat,
    PocaoCuraGrande = LargeHealPotion, PocaoForcaAlquimica = AlchemicalStrengthPotion, PocaoVenenoAlquimica = AlchemicalPoisonPotion, PocaoLentidaoAlquimica = AlchemicalSlowPotion,
    GosmaAcida = AcidSlime, DenteGoblin = GoblinTooth, NucleoPegajoso = StickyCore, PoMagico = MagicDust, MadeiraEnfeiticada = EnchantedWood, CoracaoFloresta = ForestHeart, PedraUpgrade = UpgradeStone, ConviteReal = RoyalInvitation,
    DispositivoLinguagem = LanguageDevice
};

class Character;

class Item 
{
protected:
    std::vector<Property> properties;
    int sellPrice;
    std::function<void(Character*, Character*)> useAction;
    std::function<bool(Item*, Character*, bool*)> inventoryAction;
    std::vector<std::string> inspectionDescription;

public:
    Item(int price = 3) : sellPrice(price) {}
    virtual ~Item() = default;

    virtual std::string getItemName() const = 0;
    virtual std::string getNameItem() const { return getItemName(); }

    virtual EquipmentType getType() const { return EquipmentType::None; }
    virtual EquipmentType obterTipo() const { return getType(); }

    virtual int getPhysicalDamage() const { return 0; }
    virtual int obterDanoFisico() const { return getPhysicalDamage(); }

    virtual int getMagicalDamage() const { return 0; }
    virtual int obterDanoMagico() const { return getMagicalDamage(); }

    virtual double getPercentageReduction() const { return 0.0; }
    virtual double obterReducaoPercentual() const { return getPercentageReduction(); }

    virtual int getFixedReduction() const { return 0; }
    virtual int obterReducaoFixa() const { return getFixedReduction(); }

    virtual int getShieldFixedDamageReduction() const { return 0; }
    virtual int obterReducaoDanoFixaEscudo() const { return getShieldFixedDamageReduction(); }

    virtual int getShieldCurrentDurability() const { return 0; }
    virtual int obterDurabilidadeAtualEscudo() const { return getShieldCurrentDurability(); }
    
    virtual void setInspectionDescription(const std::vector<std::string>& desc) { inspectionDescription = desc; }
    virtual void setInspectionDescription(const std::string& desc) { inspectionDescription = {desc}; }
    virtual void definirDescricaoInspecao(const std::vector<std::string>& desc) { setInspectionDescription(desc); }
    virtual void definirDescricaoInspecao(const std::string& desc) { setInspectionDescription(desc); }

    static bool checkAttributeRequirements(Character* character, int reqStrength, int reqDexterity, int reqIntelligence, int reqWisdom);
    static bool verificarRequisitosAtributos(Character* character, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria) {
        return checkAttributeRequirements(character, reqForca, reqDestreza, reqInteligencia, reqSabedoria);
    }

    static bool checkArmorRequirements(Character* character, int reqResistance, int reqConstitution);
    static bool verificarRequisitosArmadura(Character* character, int reqResistencia, int reqConstituicao) {
        return checkArmorRequirements(character, reqResistencia, reqConstituicao);
    }

    static bool checkShieldRequirements(Character* character, int reqResistance, int secVal, int reqSecondary);
    static bool verificarRequisitosEscudo(Character* character, int reqResistencia, int secVal, int reqSecundario) {
        return checkShieldRequirements(character, reqResistencia, secVal, reqSecundario);
    }

    virtual bool canBeEquippedBy(Character* /*character*/) const { return true; }
    virtual bool podeSerEquipadoPor(Character* c) const { return canBeEquippedBy(c); }

    virtual bool isEquippable() const { return false; }
    virtual bool isEquipavel() const { return isEquippable(); }

    virtual std::string getRequirementMessage() const { return "\n[SISTEMA]: Attributes insuficientes para equipar " + getItemName() + "!\n"; }
    virtual std::string obterMensagemRequisito() const { return getRequirementMessage(); }
    
    virtual std::vector<std::string> getInspectionDetails(Character* /*character*/ = nullptr) const {
        std::vector<std::string> details;
        details.push_back(" > Tipo: Desconhecido");
        details.push_back(" > Descricao: Nenhuma informacao disponivel.");
        return details;
    }
    virtual std::vector<std::string> obterDetalhesInspecao(Character* c = nullptr) const { return getInspectionDetails(c); }

    virtual void changeName(const std::string& /*n*/) {}
    virtual void alterarNome(const std::string& n) { changeName(n); }

    virtual bool hasBleedEffect() const { return false; }
    virtual bool possuiEfeitoSangramento() const { return hasBleedEffect(); }

    virtual bool hasSlowEffect() const { return false; }
    virtual bool possuiEfeitoLentidao() const { return hasSlowEffect(); }

    virtual void applyBleedEffect() {}
    virtual void aplicarEfeitoSangramento() { applyBleedEffect(); }

    virtual void applySlowEffect() {}
    virtual void aplicarEfeitoLentidao() { applySlowEffect(); }

    virtual void reduceDurability(int /*qty*/) {}
    virtual void reduzirDurabilidade(int qtd) { reduceDurability(qtd); }

    virtual void increaseDurability(int /*qty*/) {}
    virtual void aumentarDurabilidade(int qtd) { increaseDurability(qtd); }
    
    virtual void beforeDealingDamage(Character* /*attacker*/, Character* /*target*/) {}
    virtual void antesDeCausarDano(Character* atk, Character* def) { beforeDealingDamage(atk, def); }

    virtual void onDealingDamage(Character* /*attacker*/, Character* /*target*/, int /*damageDealt*/) {}
    virtual void aoCausarDano(Character* atk, Character* def, int dmg) { onDealingDamage(atk, def, dmg); }

    virtual int ensureMinimumDamage(int finalDamage) { return std::max(finalDamage, 1); }
    virtual int garantirDanoMinimo(int finalDamage) { return ensureMinimumDamage(finalDamage); }

    virtual int getSellPrice() const { return sellPrice; }
    virtual int obterPrecoVenda() const { return getSellPrice(); }

    virtual std::string getStatusInfo() const { return ""; } // Vazio por padrão para itens sem status extra
    virtual std::string obterInfoStatus() const { return getStatusInfo(); }
    
    virtual void use(Character* user, Character* target) {
        if (user == nullptr || target == nullptr) return;
        if (useAction) useAction(user, target);
    }
    virtual void usar(Character* usuario, Character* alvo) { use(usuario, alvo); }

    virtual void setUseAction(std::function<void(Character*, Character*)> action) { useAction = action; }
    virtual void definirAcaoUsar(std::function<void(Character*, Character*)> acao) { setUseAction(acao); }
    
    virtual void setInventoryAction(std::function<bool(Item*, Character*, bool*)> action) { inventoryAction = action; }
    virtual void definirAcaoInventario(std::function<bool(Item*, Character*, bool*)> acao) { setInventoryAction(acao); }

    virtual bool useFromInventory(Character* user, bool* turnConsumed) {
        if (inventoryAction) return inventoryAction(this, user, turnConsumed);
        return false;
    }
    virtual bool usarDoInventario(Character* usuario, bool* turnoFoiConsumido) { return useFromInventory(usuario, turnoFoiConsumido); }

    virtual bool hasProperty(Property prop) const { 
        return std::find(properties.begin(), properties.end(), prop) != properties.end(); 
    }
    virtual bool temPropriedade(Property prop) const { return hasProperty(prop); }

    virtual void addProperty(Property prop) { 
        if (!hasProperty(prop)) properties.push_back(prop); 
    }
    virtual void adicionarPropriedade(Property prop) { addProperty(prop); }

    virtual void removeProperty(Property prop) { 
        auto it = std::find(properties.begin(), properties.end(), prop);
        if (it != properties.end()) properties.erase(it); 
    }
    virtual void removerPropriedade(Property prop) { removeProperty(prop); }

    virtual const std::vector<Property>& getProperties() const { return properties; }
    virtual const std::vector<Property>& obterPropriedades() const { return getProperties(); }

    virtual std::unique_ptr<Item> generateUpgradedCopy() const { return nullptr; }
    virtual std::unique_ptr<Item> gerarCopiaMelhorada() const { return generateUpgradedCopy(); }
};
