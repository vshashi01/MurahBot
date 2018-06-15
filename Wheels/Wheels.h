// Wheels.h
//#include <>


#ifndef _WHEELS_h
#define _WHEELS_h




enum RobotDriveState : uint8_t {
	ROBOT_NOT_MOVING, ROBOT_FORWARD, ROBOT_BACKWARD, ROBOT_TURN_LEFT, ROBOT_TURN_RIGHT
}; //all the robot drive states that are relevant to the robot


// class to initialize the wheels of the robot. ONE instance for EACH wheel!!
class Wheel {

public:
	Wheel(int pin1 = NULL, int pin2 = NULL, int pinSetSpeed = NULL,
		int minWheelAbsoluteSpeed = 120, int maxWheelAbsoluteSpeed = 255); //default constructor with default absolute speed values
	
	Wheel(const Wheel& AWheel); //copy constructor 
	
	void initWheel();  //initialization function, called in the constructor 
	
	int pinForward; // pin that turns wheel forward with a High (relative to the robot)
	int pinBackward; //pin that turns wheel backward with a High (relative to the robot)
	int pinSetSpeed; //pin that controls motor speed, analogWrite()
	
	enum WheelState : uint8_t {
		WHEEL_NO_TURN, WHEEL_TURN_FORWARD, WHEEL_TURN_BACKWARD
	};
	
	Wheel::WheelState turnState; // tracks the state/direction of turn for wheel
	
	int minWheelAbsoluteSpeed;  //lowest speed the wheel can turn 
	int maxWheelAbsoluteSpeed;	//highest speed the wheel can turn 
		
};

//base class for the basic Drive functions. This class need not be instantiated explicitly.
class DriveWheel {
public:
	DriveWheel();
protected:
	
	void turnForward(Wheel* wheel, int speed);

	void turnBackward(Wheel* wheel, int speed);

	void stopTurning(Wheel* wheel);

};


class Drive4Wheel: public DriveWheel{
public:
	Drive4Wheel(Wheel* LeftFrontWheel, Wheel* RightFrontWheel,
		Wheel* LeftRearWheel, Wheel* RightRearWheel, int speedToleranceRange); //default constructor with 4 Wheel instatiation and speed tolerance 
	//speed tolerance range ensure that the wheel speeds are clipped below that range from the absolute max and min  
	static int maxDriveSpeed;
	static int minDriveSpeed;
	//approximately the actual robot wheel speeds allowed  
	void initDrive4Wheel();  //initialize the drive speed for the drive4wheel object 
	void goForward(int speed = minDriveSpeed);

	void goBackward(int speed = minDriveSpeed);

	void turnLeft(int leftWheelSpeed = minDriveSpeed,
		int rightWheelSpeed = minDriveSpeed);

	void turnRight(int leftWheelSpeed = minDriveSpeed,
		int rightWheelSpeed = minDriveSpeed);

	void stop();


	
	void swayLeft(int leftWheelSpeed = minDriveSpeed,
					int rightWheelSpeed = maxDriveSpeed, 
						bool reverse = false); //only to be used with joystick controls 

	void swayRight(int leftWheelSpeed = maxDriveSpeed,
					int rightWheelSpeed = minDriveSpeed,
						bool reverse = false); //only to be used with joystick controls

	RobotDriveState robotDriveState(); //return to robot drive state based on the wheel spin conditions



private:
	int _speedToleranceRange;
	Wheel* _LeftFrontWheel;
	Wheel* _RightFrontWheel;
	Wheel* _LeftRearWheel;
	Wheel* _RightRearWheel;
	
};


#endif





