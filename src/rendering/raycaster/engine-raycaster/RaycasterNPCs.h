#pragma once
#include <map>
#include "RaycasterSprites.h"

class RaycasterNPCs {
public:
    static void initializeSprites(std::map<char, SpriteCache>& cache);
    static const std::map<char, SpriteCache>& obterCacheGlobal();
};
