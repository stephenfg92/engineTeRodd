#include <ranges>
#include "AssetStore.h"

AssetStore::AssetStore(){

}

AssetStore::~AssetStore() {
    ClearAssets();
}

void AssetStore::ClearAssets() {
    // SDL tem métodos específicos para destruir texturas.
    // Por isso, não podemos usar textures.clear() logo de cara.
    // texture.second retorna o valor do par chave-valor.
    for (auto texture: textures){
        UnloadTexture(texture.second);
    }

    textures.clear();

    for (auto font : fonts) {
        UnloadFont(font.second);
    }

    fonts.clear();

    //TODO: clear tilemaps and audio.
}

// Texturas
void AssetStore::AddTexture(const std::string& assetId, const std::string& filePath){
    Image imagem = LoadImage(filePath.c_str());
    Texture2D textura = LoadTextureFromImage(imagem);
    UnloadImage(imagem);

    textures.emplace(assetId, textura);
}

Texture2D AssetStore::GetTexture(const std::string& assetId) {
    return textures[assetId];
}

// isso pode ser transformado num template! https://www.lonecpluspluscoder.com/2015/08/13/an-elegant-way-to-extract-keys-from-a-c-map/
std::vector<std::string> AssetStore::GetTextureIds() {
    // Solução: https://stackoverflow.com/questions/110157/how-to-retrieve-all-keys-or-values-from-a-stdmap-and-put-them-into-a-vector
    
    //std::vector<std::string> textureIds;
    /*for (auto const& pair : textures) {
        textureIds.push_back(pair.first);
    }*/

    auto kv = std::views::keys(textures);
    std::vector<std::string> textureIds{ kv.begin(), kv.end() };

    return textureIds;
}

//Fontes

void AssetStore::AddFont(const std::string& fontId, const std::string& filePath, int fontSize) {
    fonts.emplace(fontId, LoadFontEx(filePath.c_str(), fontSize, NULL, 0));
}

Font AssetStore::GetFont(const std::string& fontId) {
    return fonts.at(fontId);
}
