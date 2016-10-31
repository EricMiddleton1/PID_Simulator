#include <iostream>
#include <cmath>
#include <string>

#include <unistd.h>

#include <SFML/Graphics.hpp>

#include "Rotor.hpp"

using namespace std;

#define WIDTH		800
#define HEIGHT	600

#define BAR_LENGTH	200

#define BASE_THRUST	128

int main() {
	double radius, mass, thrust, dt;
	
	double kp, ki, kd;
	double target;

	cout << "Radius=";
	cin >> radius;

	cout << "Mass=";
	cin >> mass;

	cout << "Max thrust=";
	cin >> thrust;

	cout << "Time step=";
	cin >> dt;

	cout << "Kp=";
	cin >> kp;
	
	cout << "Ki=";
	cin >> ki;

	cout << "Kd=";
	cin >> kd;

	cout << "Target Angle=";
	cin >> target;

	target *= 3.14 / 180.;

	Rotor rotor(radius, mass, thrust);

	sf::RenderWindow window(sf::VideoMode(800, 600), "Single Arm PID");

	while(window.isOpen()) {
		//Check the event loop
		sf::Event event;
		while(window.pollEvent(event)) {
			if(event.type == sf::Event::Closed)
				window.close();
		}

		static double error = 0., i = 0.;

		double theta = rotor.getAngle();

		double newError = (theta - target);

		i += newError*dt;

		if(i > 1)
			i = 1;
		if(i < -1)
			i = -1;

		double pidOut = (kp*newError + ki*i + kd*(newError - error)/dt);

		error = newError;

		rotor.setMotors(BASE_THRUST + pidOut, BASE_THRUST - pidOut);
		rotor.physicsUpdate(dt);

		//Create the bar
		double cx = WIDTH/2., cy = HEIGHT/2.;
		sf::Vertex bar[] = {
			sf::Vertex(sf::Vector2f(cx - BAR_LENGTH*cos(theta),
				cy + BAR_LENGTH*sin(theta)), sf::Color(0, 0, 255)),
			sf::Vertex(sf::Vector2f(cx + BAR_LENGTH*cos(theta),
				cy - BAR_LENGTH*sin(theta)), sf::Color(0, 0, 255))
		};

		//Clear the screen
		window.clear(sf::Color::Black);

		//Draw the bar
		window.draw(bar, 2, sf::Lines);

		//Update the window
		window.display();

		//cout << "Angle: " << theta * 180./3.14 << endl;;

		sf::sleep(sf::milliseconds(1000. * dt));
	}

	cout << "P: " << kp << "\tI: " << ki << "\tD: " << kd << endl;

	return 0;
}
