#pragma once
#include <map>
#include "RaycasterSprites.h"

class RaycasterInimigos {
public:
    static void initializeSprites(std::map<char, SpriteCache>& cache);
};
