#include "entity.hpp"

my::Entity::Entity(std::shared_ptr<sf::Texture> texture_ptr) : texture_ptr{ texture_ptr } {
    if (texture_ptr) {
        sprite.setTexture(*texture_ptr);
        rect.setSize(sf::Vector2f{ texture_ptr->getSize() });
    }
    speed = 10;
    move_percent = 1;
}
const sf::Vector2f& my::Entity::getSize() {
    return rect.getSize();
}
void my::Entity::setSize(float x, float y) {
    setSize(sf::Vector2f{ x, y });
}
void my::Entity::setSize(const sf::Vector2f& size) {
    if (texture_ptr) {
        sprite.setScale(size.x / texture_ptr->getSize().x, size.y / texture_ptr->getSize().y);
    }
    rect.setSize(size);
}
const sf::Color& my::Entity::getFillColor() {
    return rect.getFillColor();
}
void my::Entity::setFillColor(const sf::Color & color) {
    if (texture_ptr) {
        sprite.setColor(color);
    }
    rect.setFillColor(color);
}
const sf::Color my::Entity::getOutlineColor() {
    return rect.getOutlineColor();
}
void my::Entity::setOutlineColor(const sf::Color & color) {
    rect.setOutlineColor(color);
}
float my::Entity::getOutlineThickness() {
    return rect.getOutlineThickness();
}
void my::Entity::setOutlineThickness(float thickness) {
    rect.setOutlineThickness(thickness);
}
void my::Entity::setPosition(float x, float y, bool fixed) {
    if (texture_ptr) {
        sprite.setPosition(x, y);
    }
    rect.setPosition(x, y);
    if (fixed)
        old_position = rect.getPosition();
}
void my::Entity::setPosition(const sf::Vector2f position, bool fixed) {
    if (texture_ptr) {
        sprite.setPosition(position);
    }
    rect.setPosition(position);
    if (fixed)
        old_position = rect.getPosition();
}
const sf::Vector2f my::Entity::getPosition() {
    return rect.getPosition();
}
void my::Entity::setPositionBy(const sf::Vector2f& position, bool fixed) {
    setPosition(getPosition() + position, fixed);
}
void my::Entity::setPositionBy(float x, float y, bool fixed) {
    x = getPosition().x + x;
    y = getPosition().y + y;
    setPosition(x, y, fixed);
}
void my::Entity::move(const sf::Vector2f & position) {
    target_move = position;
    old_position = getPosition();
}
void my::Entity::move(float x, float y) {
    target_move.x = x;
    target_move.y = y;
    old_position = getPosition();
}
void my::Entity::moveBy(const sf::Vector2f & position) {
    target_move += position;
    old_position = getPosition();
}
void my::Entity::moveBy(float x, float y) {
    target_move.x += x;
    target_move.y += y;
    old_position = getPosition();
}
const sf::Vector2f my::Entity::moving() {
    return static_cast<sf::Vector2f>(static_cast<sf::Vector2i>(target_move - old_position));
}
bool my::Entity::isMoving() {
    sf::Vector2f d{ moving() };
    return d.x || d.y;
}
void my::Entity::move(float delta) {
    if (isMoving()) {
        sf::Vector2f d{ moving() };
        setPositionBy(d.x * delta, d.y * delta, false);
        d = static_cast<sf::Vector2f>(static_cast<sf::Vector2i>(target_move - getPosition()));
        if (!d.x && !d.y)
            setPosition(target_move, true);
    }
}
// Inherited via Drawable
void my::Entity::draw(sf::RenderTarget & target, sf::RenderStates states) const {
    if (texture_ptr)
        target.draw(sprite, states);
    else
        target.draw(rect, states);
}