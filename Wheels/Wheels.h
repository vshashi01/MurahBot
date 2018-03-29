// Wheels.h


#ifndef _WHEELS_h
#define _WHEELS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define MIN_WHEEL_SPEED 120   //bound to change with more tests
#define MAX_WHEEL_SPEED 255
#define MIN_PROGRAMMABLE_WHEEL_SPEED 140 //to allow some buffer for PID controller
#define MAX_PROGRAMMABLE_WHEEL_SPEED 225
#define TURN_WHEEL_FORWARD 1
#define TURN_WHEEL_BACKWARD -1
#define NO_WHEEL_TURN 0
#define ROBOT_TURN_LEFT 1
#define ROBOT_TURN_RIGHT -1
#define ROBOT_NOT_TURNING 0



// class to initialize the wheels of the robot. ONE instance for EACH wheel!!
class Wheel {

public:
	Wheel(int pin1 = NULL, int pin2 = NULL, int enable = NULL); //default constructor
	Wheel(const Wheel& AWheel); //copy constructor 
	void initWheel();  //initialization function, called in the constructor 
	int pinForward; // pin that turns wheel forward with a High (relative to the robot)
	int pinBackward; //pin that turns wheel backward with a High (relative to the robot)
	int pinSpeed; //pin that controls motor speed, analogWrite()
	int turnState; // 1 for forward, 0 for No Turn , -1 for backward 

};

//base class for the basic Drive functons. This class need not be declared explicitly.

class DriveWheel {
public:
	DriveWheel();
protected:
	
	void turnForward(Wheel& wheel, int speed);
	void turnBackward(Wheel& wheel, int speed);
	void stopTurning(Wheel& wheel);

};


class Drive4Wheel: public DriveWheel{
public:
	Drive4Wheel(Wheel& LeftFrontWheel, Wheel& RightFrontWheel,
		Wheel& LeftRearWheel, Wheel& RightRearWheel);
	void goForward(int speed = MIN_PROGRAMMABLE_WHEEL_SPEED);
	void goBackward(int speed = MIN_PROGRAMMABLE_WHEEL_SPEED);
	void turnLeft(int leftWheelSpeed = MIN_PROGRAMMABLE_WHEEL_SPEED,
		int rightWheelSpeed = MIN_PROGRAMMABLE_WHEEL_SPEED);
	void turnRight(int leftWheelSpeed = MIN_PROGRAMMABLE_WHEEL_SPEED, 
		int rightWheelSpeed = MIN_PROGRAMMABLE_WHEEL_SPEED);
	void stop();
	int robotTurnState();
	//void swayLeft(int swaySpeed);    //implement if needed in the future 
	//void swayRight(int swaySpeed);


private:
	Wheel _LeftFrontWheel;
	Wheel _RightFrontWheel;
	Wheel _LeftRearWheel;
	Wheel _RightRearWheel;

};


#endif





