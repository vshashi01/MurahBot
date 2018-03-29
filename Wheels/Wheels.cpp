// 
// 
// 

#include <Arduino.h>
#include "Wheels.h"


Wheel::Wheel(int pin1, int pin2, int pinSpeed)
	:pinForward(pin1), pinBackward(pin2), pinSpeed(pinSpeed) {
	initWheel();
}

Wheel::Wheel(const Wheel& AWheel) {
	pinForward = AWheel.pinForward;
	pinBackward = AWheel.pinBackward;
	pinSpeed = AWheel.pinSpeed;
}

void Wheel::initWheel() {
	turnState = NO_WHEEL_TURN;
	pinMode(pinForward, OUTPUT);
	pinMode(pinBackward, OUTPUT);
	pinMode(pinSpeed, OUTPUT);
}

Drive4Wheel::Drive4Wheel(Wheel& LeftFrontWheel, Wheel& RightFrontWheel,
	Wheel& LeftRearWheel, Wheel& RightRearWheel)
	:_LeftFrontWheel(LeftFrontWheel), _RightFrontWheel(RightFrontWheel),
	_LeftRearWheel(LeftRearWheel), _RightRearWheel(RightRearWheel) {}

void Drive4Wheel::goForward(int speed) {
	turnForward(_LeftFrontWheel, speed);
	turnForward(_LeftRearWheel, speed);
	turnForward(_RightFrontWheel, speed);
	turnForward(_RightRearWheel, speed);
}

void Drive4Wheel::goBackward(int speed) {
	turnBackward(_LeftFrontWheel, speed);
	turnBackward(_RightFrontWheel, speed);
	turnBackward(_LeftRearWheel, speed);
	turnBackward(_RightRearWheel, speed);
}

void Drive4Wheel::turnLeft(int leftWheelSpeed, int rightWheelSpeed) {
	turnForward(_RightFrontWheel, rightWheelSpeed);
	turnForward(_RightRearWheel, rightWheelSpeed);
	turnBackward(_LeftFrontWheel, leftWheelSpeed);
	turnBackward(_LeftRearWheel, leftWheelSpeed);
}

void Drive4Wheel::turnRight(int leftWheelSpeed, int rightWheelSpeed) {
	turnForward(_LeftFrontWheel, leftWheelSpeed);
	turnForward(_LeftRearWheel, leftWheelSpeed);
	turnBackward(_RightFrontWheel,rightWheelSpeed);
	turnBackward(_RightRearWheel, rightWheelSpeed);
}

void Drive4Wheel::stop() {
	stopTurning(_LeftFrontWheel);
	stopTurning(_RightFrontWheel);
	stopTurning(_RightRearWheel);
	stopTurning(_LeftRearWheel);
}

int Drive4Wheel::robotTurnState() {
	if (_LeftFrontWheel.turnState == TURN_WHEEL_FORWARD &&
		_LeftRearWheel.turnState == TURN_WHEEL_FORWARD &&
		_RightFrontWheel.turnState == TURN_WHEEL_BACKWARD &&
		_RightRearWheel.turnState == TURN_WHEEL_BACKWARD)
		return ROBOT_TURN_RIGHT;

	else if (_LeftFrontWheel.turnState == TURN_WHEEL_BACKWARD &&
		_LeftRearWheel.turnState == TURN_WHEEL_BACKWARD &&
		_RightFrontWheel.turnState == TURN_WHEEL_FORWARD &&
		_RightRearWheel.turnState == TURN_WHEEL_FORWARD)
		return ROBOT_TURN_LEFT;

	else return ROBOT_NOT_TURNING;
}


DriveWheel::DriveWheel() {}

void DriveWheel::turnForward(Wheel& wheel, int speed) {
	if (speed < MIN_WHEEL_SPEED) speed = MIN_WHEEL_SPEED;
	else if (speed > MAX_WHEEL_SPEED) speed = MAX_WHEEL_SPEED;
	else;
	digitalWrite(wheel.pinForward, HIGH);
	digitalWrite(wheel.pinBackward, LOW);
	analogWrite(wheel.pinSpeed, speed);
	wheel.turnState = TURN_WHEEL_FORWARD;
}

void DriveWheel::turnBackward(Wheel& wheel, int speed) {
	if (speed < MIN_WHEEL_SPEED) speed = MIN_WHEEL_SPEED;
	else if (speed > MAX_WHEEL_SPEED) speed = MAX_WHEEL_SPEED;
	else;
	digitalWrite(wheel.pinForward, LOW);
	digitalWrite(wheel.pinBackward, HIGH);
	analogWrite(wheel.pinSpeed, speed);
	wheel.turnState = TURN_WHEEL_BACKWARD;
}

void DriveWheel::stopTurning(Wheel& wheel) {
	analogWrite(wheel.pinSpeed, 0);
	digitalWrite(wheel.pinForward, LOW);
	digitalWrite(wheel.pinBackward, LOW);
	wheel.turnState = NO_WHEEL_TURN;
}
