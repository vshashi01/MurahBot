// Wheels.h
//#include <>


#ifndef _WHEELS_h
#define _WHEELS_h





enum MinMaxRange : uint8_t {
	MIN, MAX
}; //generalized enum to denote min max quantifiers

// class to initialize the wheels of the robot. ONE instance for EACH wheel!!
class Wheel {

public:
	Wheel(int pin1 = NULL, int pin2 = NULL, int pinSetSpeed = NULL,
		int minWheelAbsoluteSpeed = 120, int maxWheelAbsoluteSpeed = 255); //default constructor with default absolute speed values
	
	Wheel(const Wheel& AWheel); //copy constructor 
	
	void initWheel();  //initialization function, called in the constructor 
	
	
	enum WheelState : uint8_t {
		WHEEL_NO_SPIN, WHEEL_SPIN_FORWARD, WHEEL_SPIN_BACKWARD
	}; //Wheel State enums to track states 

	Wheel::WheelState getCurrentWheelState(); //return the current state in _spinState 
	
	void setSpinForward(int speed); //set forward spin 
	void setSpinBackward(int speed); //set backward spin 
	void setSpinStop(); //stop spin

	int getWheelAbsoluteSpeed(MinMaxRange rangeValue); //return _minWheelAbsoluteSpeed / _maxAbsoluteSpeed
	void setWheelAbsoluteSpeed(int minSpeedAbsolute, int maxSpeedAbsolute); //resets the _min/_max Wheel Absolute Speed
	
	

private:
	int _pinForward; //pin that turns wheel forward with a High (relative to the robot)
	int _pinBackward; //pin that turns wheel backward with a High (relative to the robot)
	int _pinSetSpeed; //pin that controls motor speed, analogWrite()
	Wheel::WheelState _spinState; // tracks the state/direction of wheel spin
	int _minWheelAbsoluteSpeed;  //lowest speed the wheel can turn 
	int _maxWheelAbsoluteSpeed;	//highest speed the wheel can turn 

	int limitWheelSpeed(int wheelSpeed); //checks if wheel speed within absolute range, if not clips it 
			
};



class Drive4Wheel {
public:
	Drive4Wheel(Wheel* LeftFrontWheel, Wheel* RightFrontWheel,
		Wheel* LeftRearWheel, Wheel* RightRearWheel, int speedToleranceRange); //default constructor with 4 Wheel instatiation and speed tolerance 
	//speed tolerance range ensure that the wheel speeds are clipped below that range from the absolute max and min  
	
	
	void initDrive4Wheel();  //initialize the drive speed for the drive4wheel object 

	int limitDriveSpeed(int driveSpeed); //method to check and limit 
	//methods to drive 
	void goForward(int speed);
	void goBackward(int speed);
	void goLeft(int wheelSpeed, float speedRatio = 1.0, bool reverse = false );
	void goRight(int leftWheelSpeed, float speedRatio = 1.0, bool reverse = false);
	void stop();

	enum DriveState : uint8_t {
		DRIVE_STOP, DRIVE_FORWARD, DRIVE_BACKWARD, DRIVE_LEFT, DRIVE_RIGHT, DRIVE_BACKWARD_LEFT, DRIVE_BACKWARD_RIGHT
	}; //all the robot drive states that are relevant to the Drive class 

	//methods to get and set _speedToleranceRange that updates the drive speed values 
	int getSpeedToleranceRange();
	void setSpeedToleranceRange(int speedTolerance);

	//methods to get the drive speed values and current drive state 
	int getDriveSpeed(MinMaxRange rangeValue);
	DriveState getCurrentDriveState();

private:
	int _speedToleranceRange; //the tolerance between the absolute speeds of the Wheel instance and allowable drive speed
	static int _maxDriveSpeed;
	static int _minDriveSpeed;
	DriveState _driveState; //return to robot drive state based on the wheel spin conditions

	void setDriveSpeed(); //private method to update the drive speeds with the current _speedToleranceRange value

	Wheel* _LeftFrontWheel;
	Wheel* _RightFrontWheel;
	Wheel* _LeftRearWheel;
	Wheel* _RightRearWheel;
	
};


#endif





