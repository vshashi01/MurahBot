#include <Arduino.h>
#include "Wheels.h"

//default constructor: upon instantiation all the pin values and absolute speeds are stored in class variables 
Wheel::Wheel(int pin1, int pin2, int pinSetSpeed, int minWheelAbsoluteSpeed, int maxWheelAbsoluteSpeed)
	:pinForward(pin1), pinBackward(pin2), pinSetSpeed(pinSetSpeed), 
	minWheelAbsoluteSpeed(minWheelAbsoluteSpeed), maxWheelAbsoluteSpeed(maxWheelAbsoluteSpeed)  {
	initWheel();
}

//copy constructor 
Wheel::Wheel(const Wheel& AWheel) {
	pinForward = AWheel.pinForward;
	pinBackward = AWheel.pinBackward;
	pinSetSpeed = AWheel.pinSetSpeed;
	minWheelAbsoluteSpeed = AWheel.minWheelAbsoluteSpeed;
	maxWheelAbsoluteSpeed = AWheel.maxWheelAbsoluteSpeed;
}

//called during instantiation automatically, sets-up the pins and state variables to appropriate states 
void Wheel::initWheel() {
	turnState = WHEEL_NO_TURN;
	pinMode(pinForward, OUTPUT);
	pinMode(pinBackward, OUTPUT);
	pinMode(pinSetSpeed, OUTPUT);
}

//default constructor for 4 wheel drives robot 
//stores the pointer of each wheel objects to local wheel object pointer for easy access 
//stores the speed tolerance range to set the min and max drive speed
Drive4Wheel::Drive4Wheel(Wheel* LeftFrontWheel, Wheel* RightFrontWheel,
	Wheel* LeftRearWheel, Wheel* RightRearWheel, int speedToleranceRange)
	:_LeftFrontWheel(LeftFrontWheel), _RightFrontWheel(RightFrontWheel),
	_LeftRearWheel(LeftRearWheel), _RightRearWheel(RightRearWheel), _speedToleranceRange(speedToleranceRange) {
	initDrive4Wheel();
}

//initializing the speed variables with 0 
int Drive4Wheel::minDriveSpeed = 0;
int Drive4Wheel::maxDriveSpeed = 0;

//called during the instantiation pf the Drive4Wheel class 
void Drive4Wheel::initDrive4Wheel() {
	minDriveSpeed = max(_LeftFrontWheel->minWheelAbsoluteSpeed, max(_RightFrontWheel->minWheelAbsoluteSpeed,
		max(_LeftRearWheel->minWheelAbsoluteSpeed, _RightRearWheel->minWheelAbsoluteSpeed)));
	maxDriveSpeed = min(_LeftFrontWheel->maxWheelAbsoluteSpeed, min(_RightFrontWheel->maxWheelAbsoluteSpeed,
		min(_LeftRearWheel->maxWheelAbsoluteSpeed, _RightRearWheel->maxWheelAbsoluteSpeed)));
	//the minimum and maximum drivespeeds are evaluated from each absolute speed values of the wheels. 
	minDriveSpeed = minDriveSpeed + _speedToleranceRange;
	maxDriveSpeed = maxDriveSpeed - _speedToleranceRange;
	int biggerValue;
	if (minDriveSpeed > maxDriveSpeed) {
		biggerValue = minDriveSpeed;
		minDriveSpeed = maxDriveSpeed;
		maxDriveSpeed = biggerValue;		
	}//checks if the tolerance value given causes the values to reach an incorrect range 
	//if yes the higher and lower values are reset appropriately minDriveSpeed < maxDriveSpeed 
	else;

}

//Drive4Wheel methods for driving 
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

void Drive4Wheel::swayLeft(int leftWheelSpeed,
	int rightWheelSpeed, bool reverse) {
	int speedHigh, speedLow;
	if (rightWheelSpeed > leftWheelSpeed) {
		speedHigh = rightWheelSpeed;
		speedLow = leftWheelSpeed;
	}
	else {
		speedHigh = leftWheelSpeed;
		speedLow = rightWheelSpeed;
	}
	if (reverse) {
		turnBackward(_LeftFrontWheel, speedLow);
		turnBackward(_LeftRearWheel, speedLow);
		turnBackward(_RightFrontWheel, speedHigh);
		turnBackward(_RightRearWheel, speedHigh);
	}
	else {
		turnForward(_LeftFrontWheel, speedLow);
		turnForward(_LeftRearWheel, speedLow);
		turnForward(_RightFrontWheel, speedHigh);
		turnForward(_RightRearWheel, speedHigh);
	}
}

void Drive4Wheel::swayRight(int leftWheelSpeed,
	int rightWheelSpeed, bool reverse) {
	int speedHigh, speedLow;
	if (rightWheelSpeed > leftWheelSpeed) {
		speedHigh = rightWheelSpeed;
		speedLow = leftWheelSpeed;
	}
	else {
		speedHigh = leftWheelSpeed;
		speedLow = rightWheelSpeed;
	}
	if (reverse) {
		turnBackward(_RightFrontWheel, speedLow);
		turnBackward(_RightRearWheel, speedLow);
		turnBackward(_LeftFrontWheel, speedHigh);
		turnBackward(_LeftRearWheel, speedHigh);
	}
	else {
		turnForward(_RightFrontWheel, speedLow);
		turnForward(_RightRearWheel, speedLow);
		turnForward(_LeftFrontWheel, speedHigh);
		turnForward(_LeftRearWheel, speedHigh);
	}
}

//Drive4Wheels method to identify the drive state of the drive systems
RobotDriveState Drive4Wheel::robotDriveState() {
	if (_LeftFrontWheel->turnState == Wheel::WHEEL_TURN_FORWARD &&
		_LeftRearWheel->turnState == Wheel::WHEEL_TURN_FORWARD &&
		_RightFrontWheel->turnState == Wheel::WHEEL_TURN_BACKWARD &&
		_RightRearWheel->turnState == Wheel::WHEEL_TURN_BACKWARD)
		return ROBOT_TURN_RIGHT;

	else if (_LeftFrontWheel->turnState == Wheel::WHEEL_TURN_BACKWARD &&
		_LeftRearWheel->turnState == Wheel::WHEEL_TURN_BACKWARD &&
		_RightFrontWheel->turnState == Wheel::WHEEL_TURN_FORWARD &&
		_RightRearWheel->turnState == Wheel::WHEEL_TURN_FORWARD)
		return ROBOT_TURN_LEFT;

	else if (_LeftFrontWheel->turnState == Wheel::WHEEL_TURN_FORWARD &&
		_LeftRearWheel->turnState == Wheel::WHEEL_TURN_FORWARD &&
		_RightFrontWheel->turnState == Wheel::WHEEL_TURN_FORWARD &&
		_RightRearWheel->turnState == Wheel::WHEEL_TURN_FORWARD)
		return ROBOT_FORWARD;

	else if (_LeftFrontWheel->turnState == Wheel::WHEEL_TURN_BACKWARD &&
		_LeftRearWheel->turnState == Wheel::WHEEL_TURN_BACKWARD &&
		_RightFrontWheel->turnState == Wheel::WHEEL_TURN_BACKWARD &&
		_RightRearWheel->turnState == Wheel::WHEEL_TURN_BACKWARD)
		return ROBOT_BACKWARD;


	else return ROBOT_NOT_MOVING;
}

//base class Drive: just to have generalized spin methods for wheels 
DriveWheel::DriveWheel() {}

void DriveWheel::turnForward(Wheel* wheel, int speed) {
	speed = constrain(speed, wheel->minWheelAbsoluteSpeed, wheel->maxWheelAbsoluteSpeed);
	digitalWrite(wheel->pinForward, HIGH);
	digitalWrite(wheel->pinBackward, LOW);
	analogWrite(wheel->pinSetSpeed, speed);
	wheel->turnState = Wheel::WHEEL_TURN_FORWARD;
}

void DriveWheel::turnBackward(Wheel* wheel, int speed) {
	speed = constrain(speed, wheel->minWheelAbsoluteSpeed, wheel->maxWheelAbsoluteSpeed);
	digitalWrite(wheel->pinForward, LOW);
	digitalWrite(wheel->pinBackward, HIGH);
	analogWrite(wheel->pinSetSpeed, speed);
	wheel->turnState= Wheel::WHEEL_TURN_BACKWARD;
}

void DriveWheel::stopTurning(Wheel* wheel) {
	analogWrite(wheel->pinSetSpeed, 0);
	digitalWrite(wheel->pinForward, LOW);
	digitalWrite(wheel->pinBackward, LOW);
	wheel->turnState = Wheel::WHEEL_NO_TURN;
}
