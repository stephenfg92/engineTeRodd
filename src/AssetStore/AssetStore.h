#pragma once

#include <map>
#include <string>
#include <vector>
#include <raylib.h>
#include "TilemapData.h"

class AssetStore {
    private:
        std::map<std::string, Texture2D> textures;
        std::map<std::string, Font> fonts;
        // map for audio
        std::map<std::string, TilemapData> tilemaps;

    public:
        AssetStore();
        ~AssetStore();

        void ClearAssets();
        void AddTexture(const std::string& assetId, const std::string& filePath);
        Texture2D GetTexture(const std::string& assetId);
        std::vector<std::string> GetTextureIds();

        void AddFont(const std::string& fontId, const std::string& filePath, int fontSize);
        Font GetFont(const std::string& fontId);

        // Sem implementação por enquanto
        void AddTilemap(const std::string& tilemapId, TilemapData tilemapData);
        TilemapData GetTilemap(const std::string& tilemapId);
};