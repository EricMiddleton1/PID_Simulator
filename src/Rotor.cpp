#include "Rotor.hpp"

#include <cstdio>

using namespace std;

Rotor::Rotor(double _radius, double _mass, double _maxThrust)
	:	radius{_radius}
	,	mass{_mass}
	,	maxThrust{_maxThrust}
	,	theta{0.}
	,	thetaPrime{0.}
	,	leftMotor{0}
	,	rightMotor{0} {
	
}

void Rotor::setMotors(double _left, double _right) {
	uint8_t pwmLeft = clampMotor(_left);
	uint8_t pwmRight = clampMotor(_right);

	//Convert from 8-bit value to double in m/s^2
	leftMotor = pwmLeft * maxThrust / 255.;
	rightMotor = pwmRight * maxThrust / 255.;

	printf("Left motor: %3.3f\tRight motor: %3.3f\n", leftMotor, rightMotor);
	fflush(stdout);
}

void Rotor::physicsUpdate(double dt) {
	double thetaAccel = (rightMotor - leftMotor) / (mass * radius);

	//Euler's method with 2nd order correction
	//theta += thetaPrime*dt + 0.5*thetaAccel*dt*dt;

	thetaPrime += thetaAccel * dt;
	theta += thetaPrime*dt;
}

double Rotor::getAngle() const {
	return theta;
}

uint8_t Rotor::clampMotor(double motor) {
	if(motor < 0.)
		motor = 0.;
	else if(motor > 255.)
		motor = 255.;
	
	return motor;
}
