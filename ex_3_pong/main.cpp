#include <SFML/Graphics.hpp>

#include <iostream>//For debugging
#include <array>
#include <memory>
#include <random>
#include <chrono>
#include <thread>
#include <vector>
#include <unordered_map>

namespace my {
    float paddleSpeed = 500;
    sf::Vector2f maxSpeed{ 5, 5 };//Ball velocity max speed
    //Input key bindings (sf::Keyboard::Keys will bind to these enum)
    struct Input {
        enum Key {
            None,
            Up,
            Down,
            Left,
            Right,
            Size
        };
    };

    //Ball object
    class Ball : public sf::CircleShape {
    public:
        sf::Vector2<bool> direction;
        sf::Vector2f velocity;
        Ball(float radius, const sf::Vector2f& position, const sf::Vector2f& velocity) :
            sf::CircleShape{ radius }, velocity{ velocity }{
            setPosition(position);
            setOrigin(radius / 2, radius / 2);
            //Ternary operation for acquiring direction
            direction.x = rand() % 2 ? true : false;
            direction.y = rand() % 2 ? true : false;
        }
        std::size_t getPointCount() const override {
            return sf::CircleShape::getPointCount();
        }
        sf::Vector2f getPoint(std::size_t index) const override {
            return sf::CircleShape::getPoint(index);
        }
    };

    //Wall
    class Wall : public sf::RectangleShape {
    public:
        Wall(const sf::Vector2f& size, const sf::Vector2f& position) :
            sf::RectangleShape{ size } {
            setPosition(position);
            setOrigin(size.x / 2, size.y / 2);
        }
        void move(float x, float y) {
            setPosition(getPosition().x + x, getPosition().y + y);
        }
        void move(const sf::Vector2f position) {
            setPosition(getPosition().x + position.x, getPosition().y + position.y);
        }
        std::size_t getPointCount() const override {
            return sf::RectangleShape::getPointCount();
        }
        sf::Vector2f getPoint(std::size_t index) const override {
            return sf::RectangleShape::getPoint(index);
        }
    };

    //Player
    class Paddle : public sf::RectangleShape {
    private:
    public:
        std::unordered_map<sf::Keyboard::Key, std::pair<Input::Key, bool>> inputs;
        std::size_t score;
        Paddle(const sf::Vector2f& size, const sf::Vector2f& position, const std::size_t& score) :
            sf::RectangleShape{ size }, score{ score } {
            setPosition(position);
            setOrigin(size.x / 2, size.y / 2);
        }
        void move(float x, float y) {
            setPosition(getPosition().x + x, getPosition().y + y);
        }
        void move(const sf::Vector2f position) {
            setPosition(getPosition().x + position.x, getPosition().y + position.y);
        }
        void move(my::Input::Key key, float value = 1) {
            value *= paddleSpeed;
            switch (key) {
            case Input::Up:
                setPosition(getPosition().x + 0, getPosition().y - value);
                break;
            case Input::Down:
                setPosition(getPosition().x + 0, getPosition().y + value);
                break;
            case Input::Left:
                setPosition(getPosition().x - value, getPosition().y + 0);
                break;
            case Input::Right:
                setPosition(getPosition().x + value, getPosition().y + 0);
                break;
            }
        }
        std::size_t getPointCount() const override {
            return sf::RectangleShape::getPointCount();
        }
        sf::Vector2f getPoint(std::size_t index) const override {
            return sf::RectangleShape::getPoint(index);
        }
        //Update value at key
        void setInput(sf::Keyboard::Key key, bool value) {
            auto it = inputs.find(key);
            if (it != inputs.end())
                inputs[key].second = value;
        }
        //Returns the slope value of a point relative to this objects origin.
        float getSlope(float x, float y) {
            return x - getOrigin().x ? (y - getOrigin().y) / (x - getOrigin().x) : 0;
        }
        float getSlope(const sf::Vector2f& point) {
            return point.x - getPosition().x ? (point.y - getPosition().y) / (point.x - getPosition().x) : 0;
        }
        //Returns the slope value of self relative to this objects origin.
        float getSlope() {
            return getSlope(0, 0);
        }
        //Bind Key and value
        void setInput(sf::Keyboard::Key key, Input::Key bind, bool value = false) {
            if (bind != Input::Key::None) {
                inputs[key] = std::pair<Input::Key, bool>{bind, value};
            }
            else {
                auto it = inputs.find(key);
                if (it != inputs.end())
                    inputs.erase(it);
            }
        }
        bool getInput(const sf::Keyboard::Key& key) {
            auto it = inputs.find(key);
            if (it != inputs.end())
                return inputs[key].second;
            return false;
        }
    };
}


int main() {
    //std::random_device not implemented on all compilers, using c++ system clock seed value.
    std::mt19937 rand{ static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()) };

    sf::Rect<float> screen{ 0,0,800,600 };
    sf::RenderWindow window{ sf::VideoMode{ static_cast<unsigned int>(screen.width), 
                                            static_cast<unsigned int>(screen.height)},
                             "SFML Example Pong" };

    //Entity parameters
    sf::Vector2<float> size{ screen.width / 40.f, screen.height / 5.f },
                       left_position{ size.x, screen.height / 2},
                       right_position{ screen.width - size.x, screen.height / 2 };

    //Ball parameters
    float radius = 25.f;
    sf::Vector2<float> middle_position{screen.width / 2, screen.height / 2 }, velocity{ 0,0 };

    //Wall parameter
    sf::Vector2<float> wSize{ screen.width, screen.height / 50.f };


    float dps = 60.f,   //Draws per second
          ups = 120.f,  //Updates per second
          sps = 240.f;  //Sleeps per second

    std::size_t fps = 0;

    //Time Management: 
    //  First:  Accumulated delta change
    //  Second: Limit of delta change before acting.
    std::pair<float, float> print{ 0.f, 1.f },      //Once per second
                            draw{ 0.f, 1 / dps },   //varies depending on Draws Per Second var
                            update{ 0.f, 1 / ups }; //varies depending on Updates Per Second var

    sf::Clock clock;

    //Construct left and right player
    my::Paddle left{ size, left_position, 0 }, right{ size, right_position, 0 };

    //(Array)Map first player to left and second player to right (by address)
    std::array<my::Paddle*, 2> paddles{
        &left,
        &right
    };

    paddles[0]->setInput(sf::Keyboard::W, my::Input::Up);
    paddles[0]->setInput(sf::Keyboard::S, my::Input::Down);
    //paddles[0]->setInput(sf::Keyboard::Left, my::Input::Left);
    //paddles[0]->setInput(sf::Keyboard::Right, my::Input::Right);

    paddles[1]->setInput(sf::Keyboard::Up, my::Input::Up);
    paddles[1]->setInput(sf::Keyboard::Down, my::Input::Down);
    //paddles[1]->setInput(sf::Keyboard::Left, my::Input::Left);
    //paddles[1]->setInput(sf::Keyboard::Right, my::Input::Right);

    //(Vector growable)List of balls (my value).
    std::vector<my::Ball> balls{
        my::Ball{ radius, middle_position, velocity}
    };
    std::array<my::Wall, 2> walls{
        my::Wall{ wSize, sf::Vector2f{ screen.width / 2, 0 }},
        my::Wall{ wSize, sf::Vector2f{ screen.width / 2, screen.height}},
    };
    std::cout << walls[0].getPosition().x << ' ' << walls[0].getPosition().y << '\n';
    std::cout << walls[1].getPosition().x << ' ' << walls[1].getPosition().y << '\n';

    bool running = true;
    while (running && window.isOpen()) {
        float delta = clock.getElapsedTime().asSeconds();
        clock.restart();

        print.first += delta;
        if (print.first > print.second) {
            print.first -= print.second;
            fps = 0;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || event.type == sf::Event::Closed)
                running = false;
            
            //Evaluate if directional key pressed.
            for (auto i = 0; i < paddles.size(); ++i) {
                for (const auto& input : paddles[i]->inputs) {
                    paddles[i]->setInput(input.first, sf::Keyboard::isKeyPressed(input.first));
                }
            }
        }

        update.first += delta;
        if (update.first > update.second) {
            //Update player movements: based on input map state
            for (std::size_t i = 0; i < paddles.size(); ++i) {
                for (const auto& input : paddles[i]->inputs) {
                    auto key = &input.second;
                    if (key->second)
                        paddles[i]->move(key->first, update.first);
                    for (const auto& wall : walls) {
                        if (paddles[i]->getGlobalBounds().intersects(wall.getGlobalBounds())) {
                            paddles[i]->move(key->first, -update.first);
                        }
                    }
                }
            }
            //Check Ball State
            for (auto i = 0; i < balls.size(); ++i) {

                //Check ball to screen collision state: if outside, set to center and reset.
                if (!balls[i].getGlobalBounds().intersects(screen)) {
                    if (balls[i].getPosition().x > screen.width / 2)
                        ++paddles[0]->score;
                    else
                        ++paddles[1]->score;
                    balls[i].direction.x = rand() % 2 ? true : false;
                    balls[i].direction.y = rand() % 2 ? true : false;
                    balls[i].velocity.x = 0;
                    balls[i].velocity.y = 0;
                    balls[i].setPosition(screen.width / 2, screen.height / 2);
                }

                //Check ball to wall collision state: if collision, reverse velocity and direction
                for (auto j = 0; j < walls.size(); ++j) {
                    if (balls[i].getGlobalBounds().intersects(
                        walls[j].getGlobalBounds())) {
                        balls[i].direction.y = !balls[i].direction.y;
                        balls[i].velocity.y  = -balls[i].velocity.y;
                    }
                }

                //Check ball to paddle collision state
                for (auto &paddle : paddles) {
                    if (balls[i].getGlobalBounds().intersects(
                        paddle->getGlobalBounds())) {
                        if (std::abs(paddle->getSlope()) > std::abs(paddle->getSlope(balls[i].getPosition()))) {
                            balls[i].velocity.x = -balls[i].velocity.x;
                            while(balls[i].getGlobalBounds().intersects(paddle->getGlobalBounds()))
                                balls[i].move(balls[i].velocity);
                            balls[i].direction.x = !balls[i].direction.x;
                        }
                        else{
                            balls[i].velocity.y = -balls[i].velocity.y;
                            while (balls[i].getGlobalBounds().intersects(paddle->getGlobalBounds()))
                                balls[i].move(balls[i].velocity);
                            balls[i].direction.y = !balls[i].direction.y;
                        }
                    }
                }

                //Accumulate velocity for ball:
                if (balls[i].direction.x) {
                    balls[i].velocity.x += update.first;
                    balls[i].velocity.x = balls[i].velocity.x > my::maxSpeed.x ?
                        my::maxSpeed.x : balls[i].velocity.x;
                }
                else {
                    balls[i].velocity.x -= update.first;
                    balls[i].velocity.x = balls[i].velocity.x < -my::maxSpeed.x ?
                        -my::maxSpeed.x : balls[i].velocity.x;
                }
                if (balls[i].direction.y) {
                    balls[i].velocity.y += update.first;
                    balls[i].velocity.y = balls[i].velocity.y > my::maxSpeed.y ?
                        my::maxSpeed.y : balls[i].velocity.y;
                }
                else {
                    balls[i].velocity.y -= update.first;
                    balls[i].velocity.y = balls[i].velocity.y < -my::maxSpeed.y ?
                        -my::maxSpeed.y : balls[i].velocity.y;
                }

                //Update ball position:
                balls[i].move(balls[i].velocity);
            }
            while(update.first > update.second)
                update.first -= update.second;
        }

        draw.first += delta;
        if (draw.first > draw.second) {
            window.clear();
            for (const auto& paddle : paddles)
                window.draw(*paddle);
            for (const auto& ball : balls)
                window.draw(ball);
            for (const auto& wall : walls)
                window.draw(wall);
            window.display();
            ++fps;
            while(draw.first > draw.second)
                draw.first -= draw.second;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(1000 / sps)));
    }
    window.close();
    return 0;
}
