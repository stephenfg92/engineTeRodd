#pragma once

class Window {
public:
    virtual ~Window() = default;
    //virtual void Show(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore); //Pure virtual method
};