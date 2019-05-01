#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <functional>

namespace my {

    class Entity : public sf::Drawable {
    private:
        std::shared_ptr<sf::Texture> texture_ptr;
        sf::Sprite sprite;
        sf::RectangleShape rect;
        sf::Vector2f target_move, target_action, old_position;
        float speed, move_percent;
    public:
        std::unordered_map<sf::Keyboard::Key, std::function<void(void)>> inputs;
        std::vector <std::function<void(float)>> events;
        Entity(std::shared_ptr<sf::Texture> texture_ptr = 0);
        const sf::Vector2f& getSize();
        void setSize(float x, float y);
        void setSize(const sf::Vector2f& size);
        const sf::Color& getFillColor();
        void setFillColor(const sf::Color& color);
        const sf::Color getOutlineColor();
        void setOutlineColor(const sf::Color& color);
        float getOutlineThickness();
        void setOutlineThickness(float thickness);
        void setPosition(float x, float y, bool fixed = true);
        void setPosition(const sf::Vector2f position, bool fixed = true);
        const sf::Vector2f getPosition();
        void setPositionBy(const sf::Vector2f& position, bool fixed = true);
        void setPositionBy(float x, float y, bool fixed = true);
        void move(const sf::Vector2f& position);
        void move(float x, float y);
        void moveBy(const sf::Vector2f& position);
        void moveBy(float x, float y);
        const sf::Vector2f moving();
        bool isMoving();
        void move(float delta);
        // Inherited via Drawable
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
}
#endif // !ENTITY_HPP