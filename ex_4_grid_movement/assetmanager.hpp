#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP
/*
    Description: Pure static class asset manager. Manages assets.
*/

#include <SFML/Graphics/Texture.hpp>
#include <memory>
#include <unordered_map>
#include <fstream>
namespace my {
    class AssetManager {
    private:
        static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> assets;
        static bool file_exists(std::string filename);
    public:
        //Returns a copy of shared pointer to texture by value: RVO > unsafe
        static std::shared_ptr<sf::Texture> load(std::string filename);
    };
}
#endif // !ASSETMANAGER_HPP
