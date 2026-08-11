#pragma once

#include <unordered_map>
#include <string>

class MapTileCache {
private:
    std::unordered_map<char, std::string> formattedTileCache;

public:
    MapTileCache() = default;
    ~MapTileCache() = default;

    static MapTileCache& getInstance() {
        static MapTileCache instance;
        return instance;
    }

    bool possuiTile(char c) const {
        return formattedTileCache.find(c) != formattedTileCache.end();
    }

    const std::string& obterTile(char c) const {
        return formattedTileCache.at(c);
    }

    void armazenarTile(char c, const std::string& formatado) {
        formattedTileCache[c] = formatado;
    }

    void limpar() {
        formattedTileCache.clear();
    }
};
