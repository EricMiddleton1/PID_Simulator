#pragma once

#include <cstdint>

#include <SFML/Graphics.hpp>

class Rotor
{
public:
	Rotor(double radius, double mass, double maxThrust, double gravity,
		double boundLeft, double boundRight, double boundTop, double boundBottom,
		double elasticity, const sf::Color& color);

	void setMotors(double left, double right);

	void physicsUpdate(double dt);

	double getAngle() const;
	double getAngularVelocity() const;

	void draw(sf::RenderWindow&, unsigned int centerX, unsigned int centerY,
		double scale) const;

	uint8_t getMotorLeft() const;
	uint8_t getMotorRight() const;

private:
	const double MOTOR_SIZE = 0.05;

	static uint8_t clampMotor(double);
	void collisionCheck();

	double radius, mass, maxThrust, gravity;
	double boundLeft, boundRight, boundTop, boundBottom;
	double elasticity;
	
	double theta, thetaPrime;
	double x, y, xPrime, yPrime;

	double leftMotor, rightMotor;

	sf::Color color;
};
