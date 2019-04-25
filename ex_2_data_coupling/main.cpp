/*
    Simple SFML project.
    Modification of example 1 with illustrations of generic container of different objects.

*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <iomanip>
#include <thread>       //Sleep to save processing time.
#include <unordered_map>//Unordered map: holds a key pair that can be accessed with 
//first and second variable names. This object is being used
//to map our input state with preset SFML enumerators.
#include <utility>//For pair: to couple our timer data
#include <memory>
#include <random>

namespace my {
    //Extending the Rectangle Shape class from SFML
    class RectangleShape : public sf::RectangleShape {
    private:
        sf::Vector2<float> velocity;
    public:
        RectangleShape(sf::Vector2<float>& size): sf::RectangleShape(size){}
        void setVelocity(float x, float y) {
            this->velocity.x = x;
            this->velocity.y = y;
        }
        void setVelocity(sf::Vector2<float> velocity) {
            this->velocity.x = velocity.x;
            this->velocity.y = velocity.y;
        }
        const sf::Vector2<float>& getVelocity() const {
            return velocity;
        }

        //Required overrides from pure virtual functions from Rectanlge Shape
        std::size_t getPointCount() const override {
            return sf::RectangleShape::getPointCount();
        }
        sf::Vector2<float> getPoint(std::size_t index) const override {
            return sf::RectangleShape::getPoint(index);
        }
    };
    class CircleShape : public sf::CircleShape {
    private:
        sf::Vector2<float> velocity;
    public:
        CircleShape(float radius = 0, std::size_t pointCount = 30): sf::CircleShape(radius,pointCount){}
        void setVelocity(float x, float y) {
            this->velocity.x = x;
            this->velocity.y = y;
        }
        void setVelocity(sf::Vector2<float> velocity) {
            this->velocity.x = velocity.x;
            this->velocity.y = velocity.y;
        }
        const sf::Vector2<float>& getVelocity() const {
            return velocity;
        }

        //Required overrides from pure virtual functions from Rectanlge Shape
        std::size_t getPointCount() const override {
            return sf::CircleShape::getPointCount();
        }
        sf::Vector2<float> getPoint(std::size_t index) const override {
            return sf::CircleShape::getPoint(index);
        }
    };
}
int main() {

    //Random seed based on current time since epoch.
    unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 rand(seed);

    //Bind our input-map on construction with key(int) and value(bool).
    std::unordered_map<sf::Keyboard::Key, bool> input{
        {sf::Keyboard::Up,false},
        {sf::Keyboard::Down,false},
        {sf::Keyboard::Left,false},
        {sf::Keyboard::Right,false},
        {sf::Keyboard::Space,false}
    };
    sf::VideoMode vmode{ 500,500 };
    //Declare the Window and set the video mode and title.
    sf::RenderWindow window(vmode, "SFML works!");

    //Create a clock to acquire delta time per cycle of loop.
    sf::Clock clock;

    sf::Vector2<float> size{ 50,50 }, velocity{ 100,100 };

    //Container for each type of our rectangle shapes: Smart pointers
    std::vector<std::shared_ptr<my::RectangleShape>> rectangles{
        std::make_shared<my::RectangleShape>(size),
        std::make_shared<my::RectangleShape>(size),
        std::make_shared<my::RectangleShape>(size)
    };

    //Container for each type of our circle shapes: Smart pointers
    std::vector<std::shared_ptr<my::CircleShape>> circles{
        std::make_shared<my::CircleShape>(size.x/2),
        std::make_shared<my::CircleShape>(size.x/2),
        std::make_shared<my::CircleShape>(size.x/2)
    };

    //Container for all of our shapes to be drawn in our program.
    std::vector<std::shared_ptr<sf::Shape>> shapes;
    shapes.reserve(20);//Reserve up to 50 units; more than enough space.

    //Add the all of the shapes to our universal container.
    shapes.insert(shapes.begin(), rectangles.begin(), rectangles.end());
    shapes.insert(shapes.begin() + rectangles.size(), circles.begin(), circles.end());

    //Initialize Enemies: Red Rectangles
    for (auto rectangle : rectangles) {
        rectangle->setVelocity(velocity);
        rectangle->setFillColor(sf::Color::Red);
        float x = rectangle->getSize().x / 2, 
              y = rectangle->getSize().y / 2;
        rectangle->setOrigin(x,y);
        rectangle->setPosition(rand() % (vmode.width - (int)x * 2) + x, 
            rand() % (vmode.height - (int)y * 2) + y);
    }

    //Initialize Player: One green Rectangle
    auto player(rectangles[0]);
    player->setFillColor(sf::Color::Green);

    //Initialize Neutrals: Yellow Circles
    for (auto circle : circles) {
        circle->setVelocity(velocity);
        circle->setFillColor(sf::Color::Yellow);
        float r = circle->getRadius();
        circle->setOrigin(r, r);
        circle->setPosition((rand() % (vmode.width - (int)r * 2)) + r, 
            (rand() % (vmode.height - (int)r * 2)) + r);
    }

    //Initialize our timers for occurance of printing, updating, drawing, and sleeping.
    //Argument is a float value representing milliseconds per second.
    std::pair<float, float> print{ 0.f, 1.f },
                            update{ 0.f, 1 / 120.f },
                            draw{ 0.f, 1 / 60.f },
                            sleep{ 0.f, 1 / 240.f };

    //Set console output to be fixed and right aligned.
    std::cout << std::fixed;

    //Frame counter per second.
    size_t frames = 0;

    //Game Loop, runs while window is open.
    while (window.isOpen()) {
        //Acuire delta time.
        //Capture clock time in secons since last restart
        float delta = clock.getElapsedTime().asSeconds();
        clock.restart();//Restart clock 

        print.first += delta;//Accumulate delta time.
        if (print.first > update.second) {
            //Print to console position relative to shape.
            std::cout << std::right << std::setw(10) << std::setprecision(2) << player->getPosition().x
                << std::right << std::setw(10) << player->getPosition().y
                << std::right << std::setw(5) << frames << '\n';
            print.first -= print.second;//decrement timer by 1 second. Set to zero if no catchup.
            frames = 0;
        }

        //Declare an event variable.
        sf::Event event;

        //Poll event variables state.
        while (window.pollEvent(event)) {
            //Enter loop if there are events occuring or still available.

            //Check if window is closed or if 'escape' key was pressed.
            if (event.type == sf::Event::Closed ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            //Otherwise, iterate through our map and determine if buttons were pressed
            //If buttons were pressed, set button state to true, else set button state of false.
            for (auto pair : input) {
                if (sf::Keyboard::isKeyPressed(pair.first))
                    input[pair.first] = true;
                else
                    input[pair.first] = false;
            }
        }

        //Check if time to update:
        update.first += delta;
        if (update.first > update.second) {
            //Evaluate button states
            for (auto pair : input)
                if (pair.second) {
                    switch (pair.first) {
                    case sf::Keyboard::Up:
                        player->move(0, -player->getVelocity().y * update.first);//move by a rate of vel mul delta
                        break;
                    case sf::Keyboard::Down:
                        player->move(0, player->getVelocity().y * update.first);
                        break;
                    case sf::Keyboard::Left:
                        player->move(-player->getVelocity().y * update.first, 0);
                        break;
                    case sf::Keyboard::Right:
                        player->move(player->getVelocity().y * update.first, 0);
                        break;
                    case sf::Keyboard::Space:
                        player->rotate(100.f * update.first);
                        break;
                    }
                }
            update.first -= update.second;//Set to zero if no catchup.
        }

        //Check if time to draw
        draw.first += delta;
        if (draw.first > draw.second) {
            window.clear();
            for(auto &shape: shapes)
                window.draw(*shape.get());
            window.display();
            draw.first -= draw.second;//Set to zero if no catchup.
            ++frames;
        }

        sleep.first += delta;
        if (sleep.first > sleep.second) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            //sps is 1000/240 = ~4ms, this will sleep the thread for 2ms.
            sleep.first -= sleep.second;//Set to zero if no catchup.
        }
    }
    return 0;
}