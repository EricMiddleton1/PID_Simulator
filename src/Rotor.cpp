#include "Rotor.hpp"

#include <cmath>
#include <cstdio>

using namespace std;

Rotor::Rotor(double _radius, double _mass, double _maxThrust, double _gravity,
	double _boundLeft, double _boundRight, double _boundTop, double _boundBottom,
	double _elasticity, const sf::Color& _color)
	:	radius{_radius}
	,	mass{_mass}
	,	maxThrust{_maxThrust}
	,	gravity{_gravity}
	,	boundLeft{_boundLeft}
	,	boundRight{_boundRight}
	,	boundTop{_boundTop}
	,	boundBottom{_boundBottom}
	,	elasticity{_elasticity}
	,	theta{0.}
	,	thetaPrime{0.}
	,	x{0.}
	, y{boundBottom}
	,	xPrime{0.}
	, yPrime{0.}
	,	leftMotor{0}
	,	rightMotor{0}
	,	color(_color) {
	
}

void Rotor::setMotors(double _left, double _right) {
	uint8_t pwmLeft = clampMotor(_left);
	uint8_t pwmRight = clampMotor(_right);

	//Convert from 8-bit value to double in m/s^2
	leftMotor = pwmLeft * maxThrust / 255.;
	rightMotor = pwmRight * maxThrust / 255.;
}

void Rotor::physicsUpdate(double dt) {
	//Update lateral position
	double xAccel = (leftMotor + rightMotor) * -sin(theta);
	double yAccel = (leftMotor + rightMotor) * -cos(theta) - gravity;

	x += xPrime*dt + 0.5*xAccel*dt*dt;
	y += yPrime*dt + 0.5*yAccel*dt*dt;

	xPrime += xAccel*dt;
	yPrime += yAccel*dt;

	collisionCheck();

	//Update angular position
	double thetaAccel = (rightMotor - leftMotor) / (mass * radius);
	theta += thetaPrime*dt + 0.5*thetaAccel*dt*dt;
	thetaPrime += thetaAccel * dt;
}

void Rotor::collisionCheck() {
	double leftX = x - radius*cos(theta), leftY = y + radius*sin(theta);
	double rightX = x + radius*cos(theta), rightY = y - radius*sin(theta);

	bool leftHit, rightHit;

	//Check left bound
	if(leftX - MOTOR_SIZE <= boundLeft) {
		//Assume linear velocity for small dt
		x += boundLeft - (leftX - MOTOR_SIZE);
		xPrime *= -elasticity;
		//TODO: Apply torque from collision
	}
	if(rightX - MOTOR_SIZE <= boundLeft) {
		x += boundLeft - (leftX - MOTOR_SIZE);
		xPrime *= -elasticity;
	}

	//Check right bound
	if(leftX + MOTOR_SIZE >= boundRight) {
		x += boundRight - (leftX + MOTOR_SIZE);
		xPrime *= -elasticity;
	}
	if(rightX + MOTOR_SIZE >= boundRight) {
		x += boundRight - (rightX + MOTOR_SIZE);
		xPrime *= -elasticity;
	}

	//Check upper bound
	leftHit = leftY - MOTOR_SIZE <= boundTop;
	rightHit = rightY - MOTOR_SIZE <= boundTop;
	
	if(leftHit && rightHit) {
		if(leftY < rightY)
			rightHit = false;
		else
			leftHit = false;
	}
	if(leftHit) {
		y += boundTop - (leftY - MOTOR_SIZE);
		yPrime *= -elasticity;
	}
	else if(rightHit) {
		y += boundTop - (rightY - MOTOR_SIZE);
		yPrime *= -elasticity;
	}

	//Check lower bound
	leftHit = leftY + MOTOR_SIZE >= boundBottom;
	rightHit = rightY + MOTOR_SIZE >= boundBottom;

	if(leftHit && rightHit) {
		if(leftY > rightY)
			rightHit = false;
		else
			leftHit = false;
	}
	if(leftHit) {
		y += boundBottom - (leftY + MOTOR_SIZE);
		yPrime *= -elasticity;
	}
	else if(rightHit) {
		y += boundBottom - (rightY + MOTOR_SIZE);
		yPrime *= -elasticity;
	}
}


double Rotor::getAngle() const {
	return theta;
}

void Rotor::draw(sf::RenderWindow& _window, unsigned int _centerX,
	unsigned int _centerY, double _scale) const {

	//Boundaries
	sf::Color boundColor(255, 0, 0);
	sf::Vertex leftBoundLine[] = {
		sf::Vertex(sf::Vector2f(_scale * boundLeft + _centerX,
			_scale * boundTop + _centerY), boundColor),
		sf::Vertex(sf::Vector2f(_scale * boundLeft + _centerX,
			_scale * boundBottom + _centerY), boundColor)
	};
	sf::Vertex rightBoundLine[] = {
		sf::Vertex(sf::Vector2f(_scale * boundRight + _centerX,
			_scale * boundTop + _centerY), boundColor),
		sf::Vertex(sf::Vector2f(_scale * boundRight + _centerX,
			_scale * boundBottom + _centerY), boundColor)
	};
	sf::Vertex topBoundLine[] = {
		sf::Vertex(sf::Vector2f(_scale * boundLeft + _centerX,
			_scale * boundTop + _centerY), boundColor),
		sf::Vertex(sf::Vector2f(_scale * boundRight + _centerX,
			_scale * boundTop + _centerY), boundColor)
	};
	sf::Vertex bottomBoundLine[] = {
		sf::Vertex(sf::Vector2f(_scale * boundLeft + _centerX,
			_scale * boundBottom + _centerY), boundColor),
		sf::Vertex(sf::Vector2f(_scale * boundRight + _centerX,
			_scale * boundBottom + _centerY), boundColor)
	};


	_centerX += x * _scale;
	_centerY += y * _scale;

	//Motor positions
	sf::Vector2f leftMotorPos(_centerX - _scale*radius*cos(theta),
			_centerY + _scale*radius*sin(theta));
	sf::Vector2f rightMotorPos(_centerX + _scale*radius*cos(theta),
			_centerY - _scale*radius*sin(theta));

	//Bar
	sf::Vertex bar[] = { sf::Vertex(leftMotorPos, color),
		sf::Vertex(rightMotorPos, color)
	};

	//Motors
	float motorSize = _scale * MOTOR_SIZE;
	sf::CircleShape leftCircle(motorSize),
		rightCircle(motorSize);

	leftCircle.setOrigin(motorSize, motorSize);
	rightCircle.setOrigin(motorSize, motorSize);
	
	leftCircle.setPosition(leftMotorPos);
	rightCircle.setPosition(rightMotorPos);

	leftCircle.setOutlineColor(color);
	leftCircle.setFillColor(color);
	
	rightCircle.setOutlineColor(color);
	rightCircle.setFillColor(color);

	//Thrust vectors
	double thrustX = -sin(theta),
		thrustY = -cos(theta);
	double leftThrustLength = _scale * radius * leftMotor / maxThrust,
		rightThrustLength = _scale * radius * rightMotor / maxThrust;
	
	sf::Vertex leftVector[] = {{leftMotorPos - sf::Vector2f(2*motorSize*thrustX,
		2*motorSize*thrustY), color},
		{leftMotorPos - sf::Vector2f((2*motorSize + leftThrustLength)*thrustX,
			(2*motorSize + leftThrustLength)*thrustY), color}
	};
	sf::Vertex rightVector[] = {{rightMotorPos - sf::Vector2f(2*motorSize*thrustX,
		2*motorSize*thrustY), color},
		{rightMotorPos - sf::Vector2f((2*motorSize + rightThrustLength)*thrustX,
			(2*motorSize + rightThrustLength)*thrustY), color}
	};


	//Draw everything
	_window.draw(bar, 2, sf::Lines);
	_window.draw(leftCircle);
	_window.draw(rightCircle);
	_window.draw(leftVector, 2, sf::Lines);
	_window.draw(rightVector, 2, sf::Lines);
	_window.draw(leftBoundLine, 2, sf::Lines);
	_window.draw(rightBoundLine, 2, sf::Lines);
	_window.draw(topBoundLine, 2, sf::Lines);
	_window.draw(bottomBoundLine, 2, sf::Lines);
}

uint8_t Rotor::clampMotor(double motor) {
	if(motor < 0.)
		motor = 0.;
	else if(motor > 255.)
		motor = 255.;
	
	return motor;
}
