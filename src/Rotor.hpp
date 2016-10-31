#pragma once

#include <cstdint>

class Rotor
{
public:
	Rotor(double radius, double mass, double maxThrust);

	void setMotors(uint8_t left, uint8_t right);

	void physicsUpdate(double dt);

	double getAngle() const;
private:
	double radius, mass, maxThrust;
	double theta, thetaPrime;

	uint8_t leftMotor, rightMotor;
};
