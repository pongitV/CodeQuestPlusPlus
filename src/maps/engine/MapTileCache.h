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

    bool hasTile(char c) const {
        return formattedTileCache.find(c) != formattedTileCache.end();
    }
    bool possuiTile(char c) const { return hasTile(c); }

    const std::string& getTile(char c) const {
        return formattedTileCache.at(c);
    }
    const std::string& obterTile(char c) const { return getTile(c); }

    void storeTile(char c, const std::string& formatted) {
        formattedTileCache[c] = formatted;
    }
    void armazenarTile(char c, const std::string& formatado) { storeTile(c, formatado); }

    void clear() {
        formattedTileCache.clear();
    }
    void limpar() { clear(); }
};
