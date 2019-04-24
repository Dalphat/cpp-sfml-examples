/*
	Simple SFML project.
	Simple shape with simple input detection. Intentionally not coupling data or using 
        sprites or other available objects.

	Input detection maps keyboard input of: up, down, left, right, space, and escape.
		up:		moves the shape up
		down:	moves the shape down
		left:	moves the shape left
		right:	moves the shape right
		space:	rotates the shape clockwise
		escape: closes windows - exits application

    Note:   Diagnal movments will tranvel twice as fast as directional movement.
            This can be fixed by evaluating next movement as half if both x-axis and
            y-axis are being used.
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <thread>       //Sleep to save processing time.
#include <unordered_map>//Unordered map: holds a key pair that can be accessed with 
						//first and second variable names. This object is being used
						//to map our input state with preset SFML enumerators.

int main() {
	//Bind our input map on construction with key(int) and value(bool).
	std::unordered_map<sf::Keyboard::Key, bool> input{
		{sf::Keyboard::Up,false},
		{sf::Keyboard::Down,false},
		{sf::Keyboard::Left,false},
		{sf::Keyboard::Right,false},
		{sf::Keyboard::Space,false}
	};

	//Declare the Window and set the video mode and title.
	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");

	//Create a clock to acquire delta time per cycle of loop.
	sf::Clock clock;

	//Two SFML vectors to maintain our shape size and velocity.
	sf::Vector2<float> size(50,50), velocity(100,100);

	//Declare our shape to be illustrated in our program.
	sf::RectangleShape shape(size);
	shape.setFillColor(sf::Color::Green);//Set shape color to green.
	shape.setOrigin(size.x / 2, size.y / 2);//Set shape origin to center for demonstrating rotation.


    float timer = 0, //Timer variable for console printing every second. Prints Shape Position.
          update = 0,//Timer var for updating
          draw = 0,  //Timer var for drawing.
          sleep = 0; //Timer var for sleeping.

	std::cout << std::fixed;//Set console output to be fixed and right aligned.

    const float fps = 1 / 60.f, //Expected milliseconds per frame(draw).
                tps = 1 / 120.f,//Expected milliseconds per frame(update).
                sps = 1 / 240.f;//... per frame(action).

    size_t frames = 0;//Frames count per second.

	//Game Loop, runs while window is open.
	while (window.isOpen()) {
		//Acuire delta time.
		//Capture clock time in secons since last restart
		float delta = clock.getElapsedTime().asSeconds();
		clock.restart();//Restart clock 

		timer += delta;//Accumulate delta time.
		if (timer > 1) {
			//Print to console position relative to shape.
            std::cout << std::right << std::setw(10) << std::setprecision(2) << shape.getPosition().x
                      << std::right << std::setw(10) << shape.getPosition().y
                      << std::right << std::setw(5) << frames << '\n';
			timer -= 1;//decrement timer by 1 second. Set to zero if no catchup.
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
		update += delta;
		if (update > tps) {
			//Evaluate button states
			for (auto pair : input)
				if (pair.second)
					switch (pair.first) {
					case sf::Keyboard::Up:
						shape.move(0,-velocity.y * update);//move by a rate of vel mul delta
						break;
					case sf::Keyboard::Down:
						shape.move(0, velocity.y * update);
						break;
					case sf::Keyboard::Left:
						shape.move(-velocity.y * update, 0);
						break;
					case sf::Keyboard::Right:
						shape.move( velocity.y * update, 0);
						break;
					case sf::Keyboard::Space:
						shape.rotate(10.f * update);
						break;
					}
			update -= tps;//Set to zero if no catchup.
		}
		
		//Check if time to draw
		draw += delta;
		if (draw > fps) {
			window.clear();
			window.draw(shape);
			window.display();
			draw -= fps;//Set to zero if no catchup.
            ++frames;
		}

        sleep += delta;
        if (sleep > sps) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            //sps is 1000/240 = ~4ms, this will sleep the thread for 2ms.
            sleep -= sps;//Set to zero if no catchup.
        }
	}
	return 0;
}