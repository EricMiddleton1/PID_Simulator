#include "Rotor.hpp"


Rotor::Rotor(double _radius, double _mass, double _maxThrust)
	:	radius{_radius}
	,	mass{_mass}
	,	maxThrust{_maxThrust}
	,	theta{0.}
	,	thetaPrime{0.}
	,	leftMotor{0}
	,	rightMotor{0} {
	
}

void Rotor::setMotors(uint8_t _left, uint8_t _right) {
	//Convert from 8-bit value to double in m/s^2
	leftMotor = _left * maxThrust / 255.;
	rightMotor = _right * maxThrust / 255.;
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
