#pragma once

#include <cstdint>

class Rotor
{
public:
	Rotor(double radius, double mass, double maxThrust);

	void setMotors(double left, double right);

	void physicsUpdate(double dt);

	double getAngle() const;
private:
	static uint8_t clampMotor(double);

	double radius, mass, maxThrust;
	double theta, thetaPrime;

	double leftMotor, rightMotor;
};
