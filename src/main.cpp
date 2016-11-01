#include <iostream>
#include <cmath>
#include <string>

#include <unistd.h>

#include <SFML/Graphics.hpp>

#include "Rotor.hpp"

using namespace std;

#define RADIUS	0.5
#define MASS		0.5
#define THRUST	10.
#define GRAVITY	-9.8
#define TIME_STEP	0.01

#define BOUND_LEFT	-1.5
#define BOUND_RIGHT	1.5
#define BOUND_TOP		-1.
#define BOUND_BOTTOM	1.
#define ELASTICITY	0.3

#define WIDTH		1000
#define HEIGHT	650
#define SCALE		300.

#define THRUST_DELTA	100
#define MAX_ANGLE			(5.*3.14/180.)	

int main() {
	double radius = RADIUS, mass = MASS, thrust = THRUST, dt = TIME_STEP;
	
	double kp, ki, kd;

	cout << "Kp=";
	cin >> kp;
	
	cout << "Ki=";
	cin >> ki;

	cout << "Kd=";
	cin >> kd;

	Rotor rotor(radius, mass, thrust, GRAVITY,
		BOUND_LEFT, BOUND_RIGHT, BOUND_TOP, BOUND_BOTTOM, ELASTICITY,
		sf::Color(0, 0, 255));

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Single Arm PID");

	while(window.isOpen()) {
		//Check the event loop
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}

		static double error = 0., i = 0.;
		static int thrust = 0;

		//Update thrust
		thrust += (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) - 
			sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) * THRUST_DELTA * dt;

		if(thrust > 255)
			thrust = 255;
		else if(thrust < 0)
			thrust = 0;


		//Update target angle
		double target = MAX_ANGLE*(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) -
			sf::Keyboard::isKeyPressed(sf::Keyboard::Right));

		double theta = rotor.getAngle();

		double newError = (theta - target);

		i += newError*dt;

		if(i > 10.)
			i = 10.;
		if(i < -10.)
			i = -10.;

		double pidOut = (kp*newError + ki*i + kd*(newError - error)/dt);

		error = newError;

		rotor.setMotors(thrust + pidOut, thrust - pidOut);
		rotor.physicsUpdate(dt);

		//Clear the screen
		window.clear(sf::Color::Black);

		//Draw the rotor
		rotor.draw(window, WIDTH/2, HEIGHT/2, SCALE);

		//Update the window
		window.display();

		//cout << "Angle: " << theta * 180./3.14 << endl;;

		sf::sleep(sf::milliseconds(1000. * dt));
	}

	cout << "P: " << kp << "\tI: " << ki << "\tD: " << kd << endl;

	return 0;
}
