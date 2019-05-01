/*
    Description:
        Grid Movement design without grid.
*/
#include <SFML/Graphics.hpp>
#include "my.hpp"
#include <thread>


int main() {
    sf::RenderWindow window{ sf::VideoMode{800,600},"Game" };
    std::pair<float, float> updateTimer{ 0.f,1 / 120.f }, drawTimer{ 0.f,1 / 60.f };

    std::vector<my::Entity> entities;
    entities.push_back(my::Entity(my::AssetManager::load("badlogic.jpg")));
    entities[0].setSize(100,100);
    entities[0].setOutlineColor(sf::Color::Red);
    entities[0].setOutlineThickness(-5);

    entities[0].inputs[sf::Keyboard::Up] = [&entities]()->void {
        if (!entities[0].isMoving())
            entities[0].moveBy(0, -entities[0].getSize().y);
    };
    entities[0].inputs[sf::Keyboard::Down] = [&entities]()->void {
        if (!entities[0].isMoving())
            entities[0].moveBy(0, entities[0].getSize().y);
    };
    entities[0].inputs[sf::Keyboard::Left] = [&entities]()->void {
        if (!entities[0].isMoving())
            entities[0].moveBy(-entities[0].getSize().x, 0);
    };
    entities[0].inputs[sf::Keyboard::Right] = [&entities]()->void {
        if (!entities[0].isMoving())
            entities[0].moveBy(entities[0].getSize().x, 0);
    };
    entities[0].events.push_back([&entities](float delta)->void {
        entities[0].move(delta);
    });

    bool running = true;
    while (running) {
        my::delta = my::clock.getElapsedTime().asSeconds();
        my::clock.restart();
        sf::Event event;
        while (window.pollEvent(event)) {

            if(event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                running = false;
            for (std::size_t i = 0; i < entities.size(); ++i) {
                for (const auto& input : entities[i].inputs) {
                    if (sf::Keyboard::isKeyPressed(input.first))
                        input.second();
                }
            }
        }
        updateTimer.first += my::delta;
        if (updateTimer.first > updateTimer.second) {
            for (std::size_t i = 0; i < entities.size(); ++i)
                for (const auto& event : entities[i].events)
                    event(updateTimer.first);
            while (updateTimer.first > updateTimer.second)
                updateTimer.first -= updateTimer.second;
        }
        window.clear();
        for (const auto& entity : entities)
            window.draw(entity);
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000/240)));
    }
    return 0;
}