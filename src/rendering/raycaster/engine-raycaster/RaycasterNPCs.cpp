#include "RaycasterNPCs.h"
#include "RaycasterEnemies.h"
#include "../../../entities/npcs/mage-npc/NPCMageNPCLayout.h"
#include "../../../entities/npcs/priest/NPCPriestLayout.h"
#include "../../../entities/npcs/blacksmith/NPCBlacksmithLayout.h"
#include "../../../entities/npcs/merchant/NPCMerchantLayout.h"
#include "../../../entities/npcs/appearance/NPCAppearanceLayout.h"
#include "../../../entities/npcs/generic-knight/NPCGenericKnightLayout.h"
#include "../../../entities/npcs/alchemist/NPCAlchemistLayout.h"
#include "TextureManager.h"
#include <vector>
#include <algorithm>
#include <string>
#include <cmath>
#include <map>

const std::map<char, SpriteCache>& RaycasterNPCs::obterCacheGlobal() {
    static std::map<char, SpriteCache> s_cacheGlobal;
    static bool s_inicializado = false;
    if (!s_inicializado) {
        RaycasterInimigos::initializeSprites(s_cacheGlobal);
        RaycasterNPCs::initializeSprites(s_cacheGlobal);
        s_inicializado = true;
    }
    return s_cacheGlobal;
}

void RaycasterNPCs::initializeSprites(std::map<char, SpriteCache>& cache) {
    cache['B'] = RaycasterSprites::parseSprite(NPCBlacksmithLayouts::arteBlacksmith, 100, 200, 255); // Ciano Bjorn
    cache['W'] = RaycasterSprites::parseSprite(NPCMageNPCLayouts::arteMageNPC, 200, 100, 255); // Roxo Morgana
    cache['V'] = RaycasterSprites::parseSprite(NPCMerchantLayouts::arteMerchant, 255, 200, 50); // Amarelo Franchesco
    cache['C'] = RaycasterSprites::parseSprite(NPCGenericKnightLayouts::arteCavaleiro, 200, 200, 220); // Cavaleiro Real
    cache['Z'] = RaycasterSprites::parseSprite(NPCAparenciaLayouts::arteAparencia, 120, 50, 200); // Anok (Roxo Estiloso)
    cache['Q'] = RaycasterSprites::parseSprite(NPCAlchemistLayouts::arteAlchemist, 180, 50, 200); // Alchemist (Roxo)
    cache['J'] = RaycasterSprites::parseSprite(NPCPriestLayouts::artePriest, 255, 215, 0); // Priest Benedito (Dourado)

    SpriteCache doorPng = RaycasterSprites::carregarSpritePNG("assets/doors/portalDoor.png");
    for (char c : {'^', '1', '2', '3', '4', '5'}) {
        cache[c] = doorPng;
    }

    SpriteCache treePng = RaycasterSprites::carregarSpritePNG("assets/trees/TreeVillage.png");
    if (treePng.width > 0 && treePng.height > 0) {
        cache['*'] = treePng;
    }

    SpriteCache forestTreePng = RaycasterSprites::carregarSpritePNG("assets/trees/forestTree.png");
    if (forestTreePng.width > 0 && forestTreePng.height > 0) {
        cache[127] = forestTreePng;
    }
}




