#include "assetmanager.hpp"

std::unordered_map<std::string, std::shared_ptr<sf::Texture>> my::AssetManager::assets;
bool my::AssetManager::file_exists(std::string filename) {
    std::fstream file(filename, std::fstream::in);//fstream implements RAII
    return file.good();
}
std::shared_ptr<sf::Texture> my::AssetManager::load(std::string filename) {
    if (file_exists(filename)) {
        if (assets.find(filename) == assets.end()) {
            assets[filename] = std::make_shared<sf::Texture>();
            assets[filename]->loadFromFile(filename);
        }
        return assets[filename];
    }
    return 0;
}