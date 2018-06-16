#include <Arduino.h>
#include "Wheels.h"

//default constructor: upon instantiation all the pin values and absolute speeds are stored in class variables 
Wheel::Wheel(int pin1, int pin2, int pinSetSpeed, int minWheelAbsoluteSpeed, int maxWheelAbsoluteSpeed)
	:_pinForward(pin1), _pinBackward(pin2), _pinSetSpeed(pinSetSpeed), 
	_minWheelAbsoluteSpeed(minWheelAbsoluteSpeed), _maxWheelAbsoluteSpeed(maxWheelAbsoluteSpeed)  {
	initWheel();
}

//copy constructor 
Wheel::Wheel(const Wheel& AWheel) {
	_pinForward = AWheel._pinForward;
	_pinBackward = AWheel._pinBackward;
	_pinSetSpeed = AWheel._pinSetSpeed;
	_minWheelAbsoluteSpeed = AWheel._minWheelAbsoluteSpeed;
	_maxWheelAbsoluteSpeed = AWheel._maxWheelAbsoluteSpeed;
}

//called during instantiation automatically, sets-up the pins and state variables to appropriate states 
void Wheel::initWheel() {
	_spinState = WHEEL_NO_SPIN;
	pinMode(_pinForward, OUTPUT);
	pinMode(_pinBackward, OUTPUT);
	pinMode(_pinSetSpeed, OUTPUT);
}

// returns _spinState
Wheel::WheelState Wheel::getCurrentWheelState() {
	return _spinState;
}

//set spin Forward 
void Wheel::setSpinForward(int speed) {
	digitalWrite(_pinForward, HIGH);
	digitalWrite(_pinBackward, LOW);
	analogWrite(_pinSetSpeed, speed);
	_spinState = WHEEL_SPIN_FORWARD;
}

//set spin Backward 
void Wheel::setSpinBackward(int speed) {
	digitalWrite(_pinForward, LOW);
	digitalWrite(_pinBackward, HIGH);
	analogWrite(_pinSetSpeed, speed);
	_spinState = WHEEL_SPIN_BACKWARD;
}

//set spin Stop
void Wheel::setSpinStop() {
	digitalWrite(_pinForward, LOW);
	digitalWrite(_pinBackward, LOW);
	analogWrite(_pinSetSpeed, 0);
	_spinState = WHEEL_NO_SPIN;
}

// return _max/_min Wheel Absolute Speeds 
int Wheel::getWheelAbsoluteSpeed(MinMaxRange rangeValue) {
	if (rangeValue == MIN)
		return _minWheelAbsoluteSpeed;
	else if (rangeValue == MAX)
		return _maxWheelAbsoluteSpeed;
	else
		return NULL;
}

//resets the Absolute Speed values 
void Wheel::setWheelAbsoluteSpeed(int minSpeedAbsolute, int maxSpeedAbsolute) {
	_minWheelAbsoluteSpeed = minSpeedAbsolute;
	_maxWheelAbsoluteSpeed = maxSpeedAbsolute;
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
	minDriveSpeed = max(_LeftFrontWheel->getWheelAbsoluteSpeed(MIN), max(_RightFrontWheel->getWheelAbsoluteSpeed(MIN),
		max(_LeftRearWheel->getWheelAbsoluteSpeed(MIN), _RightRearWheel->getWheelAbsoluteSpeed(MIN))));
	maxDriveSpeed = min(_LeftFrontWheel->getWheelAbsoluteSpeed(MAX), min(_RightFrontWheel->getWheelAbsoluteSpeed(MAX),
		min(_LeftRearWheel->getWheelAbsoluteSpeed(MAX), _RightRearWheel->getWheelAbsoluteSpeed(MAX))));
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
	if (_LeftFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD &&
		_LeftRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD &&
		_RightFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD &&
		_RightRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD)
		return ROBOT_TURN_RIGHT;

	else if (_LeftFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD &&
		_LeftRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD &&
		_RightFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD &&
		_RightRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD)
		return ROBOT_TURN_LEFT;

	else if (_LeftFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD &&
		_LeftRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD &&
		_RightFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD &&
		_RightRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_FORWARD)
		return ROBOT_FORWARD;

	else if (_LeftFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD &&
		_LeftRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD &&
		_RightFrontWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD &&
		_RightRearWheel->getCurrentWheelState() == Wheel::WHEEL_SPIN_BACKWARD)
		return ROBOT_BACKWARD;


	else return ROBOT_NOT_MOVING;
}

//base class Drive: just to have generalized spin methods for wheels 
DriveWheel::DriveWheel() {}

void DriveWheel::turnForward(Wheel* wheel, int speed) {
	speed = constrain(speed, wheel->getWheelAbsoluteSpeed(MIN), wheel->getWheelAbsoluteSpeed(MAX));
	wheel->setSpinForward(speed);
}

void DriveWheel::turnBackward(Wheel* wheel, int speed) {
	speed = constrain(speed, wheel->getWheelAbsoluteSpeed(MIN), wheel->getWheelAbsoluteSpeed(MAX));
	wheel->setSpinBackward(speed);
}

void DriveWheel::stopTurning(Wheel* wheel) {
	wheel->setSpinStop();
}
