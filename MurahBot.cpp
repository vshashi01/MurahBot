/*###############################################################################################################
Project Name: MurahBot

Current Version: 0.3.6


Description: A 4WD mobile robot made of 4 motors, 2 Dual H Bridge Motor
Drivers, with Arduino Mega2560 controlled with Blynk App and BLE connection with Joystick Input.
Push button as System button.
Developer: Shashi
Email: vshashi01gmail.com
GitHub:
External Libraries/App used:
1. Wheels library (self-made)
2. Task Schedular Library
3. Blynk App and Blynk libraries for Arduino
4. Bounce2 Libraries for Pushbuttons

#################################################################################################################*/

#define BLYNK_USE_DIRECT_CONNECT 

#define MurahBotBT Serial1


#include <Bounce2.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskScheduler.h>
#include <Wheels.h>
#include <Arduino.h>
#include <BlynkSimpleSerialBLE.h>


///////////////////////////////////////////////////////////////////////////
//Bluetooth and Blynk related declarations 
char auth[] = "66390b83798e4495aa9d6c23724f2181"; //Blynk Authorization code



///////////////////////////////////////////////////////////////////////////
// Wheel class and Drive4Wheel class instantiation 
Wheel* WheelFrontLeft = new Wheel(46, 47, 5);
Wheel* WheelFrontRight = new Wheel(48, 49, 4);
Wheel* WheelRearLeft = new Wheel(50, 51, 7);
Wheel* WheelRearRight = new Wheel(52, 53, 6);


int speedTolerance = 30; //range of tolerance for drive speeds
Drive4Wheel murahDrive(WheelFrontLeft, WheelFrontRight,
	WheelRearLeft, WheelRearRight, speedTolerance);


////////////////////////////////////////////////////////////////////////////////
// Button Pin initialization and Bounce class instantiation 
const byte buttonPinRobotStartStop = 22; //Start and Stop Robot 
										 //const byte buttonPinBackward = 23;
										 //const byte buttonPinLeft = 24;
										 //const byte buttonPinRight = 25;  //phased out to be repurposed in the future 

Bounce ButtonRobotStartStop(buttonPinRobotStartStop, 10);



///////////////////////////////////////////////////////////////////////////////
// Global Variable Initialization 

/*
//need to assess if needed in the future
enum ButtonState
{
Changed, Not_Changed
};
*/
enum SystemStates {
	ACTIVE, PASSIVE, NO_STATE
};
//state variables
SystemStates prevSystemState = NO_STATE;
SystemStates currSystemState = PASSIVE; // 
SystemStates currBlynkState = PASSIVE;
int ledPin = 13; //on-board LED 

///////////////////////////////////////////////////////////////////////////////
//function prototypes 

void callbackButtonState();
void callbackButtonAction();
void callbackEnableDisableDrive();
void callbackBlynk();
bool onEnableOfEnableDisableDrive();
bool onEnableBlynk();
void callbackPrimaryJoystickDrive();
void callbackSecondaryJoystickDrive();
void callbackDisplayDriveState();


///////////////////////////////////////////////////////////////////////////////
// Scheduler and Tasks instantiation
Scheduler MurahBotSchedule;
Task taskUpdateButton(TASK_IMMEDIATE, TASK_FOREVER, &callbackButtonState, &MurahBotSchedule);
Task taskEnableDisableDrive(TASK_IMMEDIATE, TASK_ONCE, &callbackEnableDisableDrive, &MurahBotSchedule, false);
Task taskDrive(50, TASK_FOREVER, &callbackPrimaryJoystickDrive, &MurahBotSchedule, false);
Task taskRunBlynk(TASK_IMMEDIATE, TASK_FOREVER, &callbackBlynk, &MurahBotSchedule, false, &onEnableBlynk);

////////////////////////////////////////////////////////////////////////////////
// updates various button states  
void callbackButtonState() {
	if (ButtonRobotStartStop.update()) {
		if (ButtonRobotStartStop.read() == HIGH) {
			if (prevSystemState == NO_STATE) {
				prevSystemState = currSystemState;
				return;
			}
			else if (prevSystemState == currSystemState) {
				taskEnableDisableDrive.enable();
				return;
			}
			else {
				taskEnableDisableDrive.restart();
				return;
			}
		}
		else;
	}
	else;
	taskUpdateButton.setCallback(&callbackButtonAction);
	//Force next iteration callbackButtonAction if needed 
}

//updates the Button Actions after the Button status are updated //if neccessary
void callbackButtonAction() {
	taskUpdateButton.setCallback(&callbackButtonState);
}

// updates the Enable and Disable the drive systems
void callbackEnableDisableDrive() {
	if (currSystemState == PASSIVE) {
		prevSystemState = currSystemState;
		currSystemState = ACTIVE; //changes the system state 
		Serial.println("Bringing drive systems online....");
		taskDrive.enable();
		if (currBlynkState == PASSIVE)taskRunBlynk.enable(); //enable only once 
	}
	else {
		prevSystemState = currSystemState;
		currSystemState = PASSIVE;
		Serial.println("Shutting down drive systems...");
		murahDrive.stop(); //force stop the robot
		taskDrive.disable();
	}
}


//callback Blynk app for update
void callbackBlynk() {
	Blynk.run();
	taskRunBlynk.setCallback(callbackBlynk);
}

//ensures the callbackEnableDisableDrive run in the next iteration
bool onEnableOfEnableDisableDrive() {
	taskEnableDisableDrive.setCallback(&callbackEnableDisableDrive);
	taskEnableDisableDrive.forceNextIteration();
	return true;
}

//ensures the Blynk app is connected 
bool onEnableBlynk() {
	Blynk.begin(auth, MurahBotBT);
	currBlynkState = ACTIVE;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//drive control variables and functions 
//to be ported to a separate .ino file in the future

//constant values for jostick pad region
const byte X_THRESHOLD_LOW = 108; //X: 128 - 20
const byte X_THRESHOLD_HIGH = 148; //X: 128 + 20   
const byte Y_THRESHOLD_LOW = 108;
const byte Y_THRESHOLD_HIGH = 148;

// initialized the X and Y values to the center position 127
int joystickX = 127;
int joystickY = 127;

//ratio range for speedRatios as a multiple of 100
float smallestRatio = 45;
float biggestRatio = 60;

//Blynk input of joystick values 
BLYNK_WRITE(V1) {
	joystickX = param[0].asInt();
	joystickY = param[1].asInt();
}

//updates the Primary controls for Joystick in four directions: Front, Back, Left, Right. 
void callbackPrimaryJoystickDrive() {
	//boolean conditions for the threshold regions
	bool joystickXInThreshold = (joystickX > X_THRESHOLD_LOW && joystickX < X_THRESHOLD_HIGH);
	bool joystickYInThreshold = (joystickY > Y_THRESHOLD_LOW && joystickY < Y_THRESHOLD_HIGH);

	//if joystick not in the threshold region sets callback to secondaryJoystickDrive
	if (!joystickXInThreshold && !joystickYInThreshold) {
		taskDrive.setCallback(&callbackSecondaryJoystickDrive);
		return;
	}

	//initializes the current allowable driveSpeeds as temp variable 
	int minSpeed = murahDrive.getDriveSpeed(MIN);
	int maxSpeed = murahDrive.getDriveSpeed(MAX);

	// temp variable 
	int speed;

	//various statement to check the appropriate Primary actions
	if (joystickXInThreshold && joystickYInThreshold) {
		murahDrive.stop();	//stop the drive if in absolute center	
	}
	else if (joystickY < Y_THRESHOLD_LOW && joystickXInThreshold) {
		speed = map(joystickY, Y_THRESHOLD_LOW, 0, minSpeed, maxSpeed); //map speed between joystick coordinate and allowable drive Speeds
		murahDrive.goBackward(speed);
	}
	else if (joystickY > Y_THRESHOLD_HIGH && joystickXInThreshold) {
		speed = map(joystickY, Y_THRESHOLD_HIGH, 255, minSpeed, maxSpeed);
		murahDrive.goForward(speed);
	}
	else if (joystickX < X_THRESHOLD_LOW && joystickYInThreshold) {
		speed = map(joystickX, X_THRESHOLD_LOW, 0, minSpeed, maxSpeed);
		murahDrive.goLeft(speed);
	}
	else if (joystickX > X_THRESHOLD_HIGH && joystickYInThreshold) {
		speed = map(joystickX, X_THRESHOLD_HIGH, 255, minSpeed, maxSpeed);
		murahDrive.goRight(speed);
	}
	else murahDrive.stop(); //just in case call stop always 

	taskDrive.setCallback(&callbackDisplayDriveState);
}

//updates the Secondary controls for Joystick: Sway Left and Right in Forward and Backward
void callbackSecondaryJoystickDrive() {
	//initializes the current allowable drive Speeds
	int minSpeed = murahDrive.getDriveSpeed(MIN);
	int maxSpeed = murahDrive.getDriveSpeed(MAX);

	//temp variables
	int speed;
	float turnRatio = 0;

	//various statements to check for the Secondary conditions
	if (joystickX < X_THRESHOLD_LOW && joystickY > Y_THRESHOLD_HIGH) {
		speed = map(joystickY, Y_THRESHOLD_HIGH, 255, minSpeed, maxSpeed);
		turnRatio = map(joystickX, X_THRESHOLD_LOW, 0, smallestRatio, biggestRatio); //map speed ratio 
		murahDrive.swayLeft(speed, (turnRatio / 100)); //ratio to be converted to float value  
	}
	else if (joystickX < X_THRESHOLD_LOW && joystickY < Y_THRESHOLD_LOW) {
		speed = map(joystickY, Y_THRESHOLD_LOW, 0, minSpeed, maxSpeed);
		turnRatio = map(joystickX, X_THRESHOLD_LOW, 0, smallestRatio, biggestRatio);
		murahDrive.swayLeft(speed, (turnRatio / 100), true);
	}
	else if (joystickX > X_THRESHOLD_HIGH && joystickY > Y_THRESHOLD_HIGH) {
		speed = map(joystickY, Y_THRESHOLD_HIGH, 255, minSpeed, maxSpeed);
		turnRatio = map(joystickX, X_THRESHOLD_HIGH, 255, smallestRatio, biggestRatio);
		murahDrive.swayRight(speed, (turnRatio / 100));
	}
	else if (joystickX > X_THRESHOLD_HIGH && joystickY < Y_THRESHOLD_LOW) {
		speed = map(joystickY, Y_THRESHOLD_LOW, 0, minSpeed, maxSpeed);
		turnRatio = map(joystickX, X_THRESHOLD_HIGH, 255, smallestRatio, biggestRatio);
		murahDrive.swayRight(speed, (turnRatio / 100), true);
	}
	else murahDrive.stop();

	taskDrive.setCallback(&callbackDisplayDriveState);

}

//update drive state to be changes to update on Blynk app in future
void callbackDisplayDriveState() {
	Serial.print("Drive State: ");
	Serial.println(murahDrive.getCurrentDriveState());

	taskDrive.setCallback(&callbackPrimaryJoystickDrive);
}



void setup() {

	pinMode(buttonPinRobotStartStop, INPUT_PULLUP); //initialize the button

	Serial.begin(9600);
	delay(500);
	MurahBotBT.begin(115200); //starts the BLE module 
	delay(100);

	//enabling the Tasks
	taskUpdateButton.enable();

	//on board LED 
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);

}

void loop() {

	MurahBotSchedule.execute();

}