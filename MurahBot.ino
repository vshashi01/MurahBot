/*##########################################################################
 Project Name: MurahBot												   
 Current Version: 0.2.2												   
 Description: A 4WD mobile robot made of 4 motors, 2 Dual H Bridge Motor  
			   Drivers, with Arduino Mega2560 connected with 4 Push        
			   Buttons.	    											   
 Developer: Shashi														   
 Email: vshashi01gmail.com												   
 GitHub:																   
 External Libraries used:												   
	1. Wheels library (self-made)										   
	2. Task Schedular Library											   
																		   
###########################################################################*/

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

Drive4Wheel murahDrive(WheelFrontLeft, WheelFrontRight,
	WheelRearLeft, WheelRearRight);

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

SystemStates prevSystemState = NO_STATE;
SystemStates currSystemState = PASSIVE; // turns on the robot and kills it //use enum to apply this 
SystemStates currBlynkState = PASSIVE;
int ledPin = 13; 




///////////////////////////////////////////////////////////////////////////////
// Scheduler and Tasks instantiation
Scheduler MurahBotSchedule;
Task taskUpdateButton(TASK_IMMEDIATE, TASK_FOREVER, &callbackButtonState, &MurahBotSchedule);
Task taskEnableDisableMurahBot(TASK_IMMEDIATE, TASK_ONCE, &callbackEnableDisableMurahBot, &MurahBotSchedule, false);
Task taskUpdateDriveState(50, TASK_FOREVER, &callbackJoystickDrive, &MurahBotSchedule, false);
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
				taskEnableDisableMurahBot.enable();
				return;
			}
			else {
				taskEnableDisableMurahBot.restart();
				return;
			}
		}
		else;
	}
	else;
	taskUpdateButton.setCallback(&callbackButtonAction);
		//Force next iteration callbackButtonAction if needed 
}

// updates the Button Actions after the Button is pressed 
void callbackButtonAction() {
	taskUpdateButton.setCallback(&callbackButtonState);
}

void callbackEnableDisableMurahBot() {
	if (currSystemState == PASSIVE) {
		prevSystemState = currSystemState;
		currSystemState = ACTIVE;
		Serial.println("Bringing drive systems online....");  
		taskUpdateDriveState.enable();
		if (currBlynkState == PASSIVE)taskRunBlynk.enable();
		else;
	}
	else {
		prevSystemState = currSystemState;
		currSystemState = PASSIVE;
		Serial.println("Shutting down drive systems...");
		taskUpdateDriveState.disable();
	}
	
}

bool onEnableOfEnableDisableMurahBot() {
	taskEnableDisableMurahBot.setCallback(&callbackEnableDisableMurahBot);
	taskEnableDisableMurahBot.forceNextIteration();
	return true;
}

void callbackJoystickDrive() {
	joystickDrive();
	taskUpdateDriveState.setCallback(&callbackJoystickDrive);
}

// runs Blynk
void callbackBlynk() {
	Blynk.run();
	taskRunBlynk.setCallback(callbackBlynk);
}

bool onEnableBlynk() {
	Blynk.begin(auth, MurahBotBT);
	currBlynkState = ACTIVE;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//joystick control variables and functions 
const byte X_THRESHOLD_LOW = 108; //X: 128 - 20
const byte X_THRESHOLD_HIGH = 148; //X: 128 + 20   
const byte Y_THRESHOLD_LOW = 108;
const byte Y_THRESHOLD_HIGH = 148;

int joystickX = 127; // initialized to the center position 127
int joystickY = 127; // initilized to the center position

int dirStateX; // defined using Wheels Macros 
int dirStateY; // to keep robot wheel action state in 2 directions  

byte speedRun; //forward backward speed 
byte speedTurn; //turn speed 

BLYNK_WRITE(V1) {
	joystickX = param[0].asInt();
	joystickY = param[1].asInt();
	//Serial.println("Joystick Moved");
}

void mapJoystick() {
	if (joystickX < X_THRESHOLD_LOW) {
		dirStateX = ROBOT_TURN_LEFT;
		speedTurn = map(joystickX, X_THRESHOLD_LOW, 0,
			MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		if (joystickY < Y_THRESHOLD_LOW) {
			dirStateY = ROBOT_MOVING_BACKWARD;
			speedRun = (joystickY, Y_THRESHOLD_LOW, 0,
				MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		}
		else if (joystickY > Y_THRESHOLD_HIGH) {
			dirStateY = ROBOT_MOVING_FORWARD;
			speedRun = (joystickY, Y_THRESHOLD_HIGH, 255,
				MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		}
		else {
			dirStateY = ROBOT_NOT_MOVING;
			speedRun = 0;
		}
	}
	else if (joystickX > X_THRESHOLD_HIGH) {
		dirStateX = ROBOT_TURN_RIGHT;
		speedTurn = map(joystickX, X_THRESHOLD_HIGH, 255,
			MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		if (joystickY < Y_THRESHOLD_LOW) {
			dirStateY = ROBOT_MOVING_BACKWARD;
			speedRun = (joystickY, Y_THRESHOLD_LOW, 0,
				MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		}
		else if (joystickY > Y_THRESHOLD_HIGH) {
			dirStateY = ROBOT_MOVING_FORWARD;
			speedRun = (joystickY, Y_THRESHOLD_HIGH, 255,
				MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		}
		else {
			dirStateY = ROBOT_NOT_MOVING;
			speedRun = 0;
		}
	}
	else {
		dirStateX = ROBOT_NOT_MOVING;
		speedTurn = 0;
		if (joystickY < Y_THRESHOLD_LOW) {
			dirStateY = ROBOT_MOVING_BACKWARD;
			speedRun = (joystickY, Y_THRESHOLD_LOW, 0,
				MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		}
		else if (joystickY > Y_THRESHOLD_HIGH) {
			dirStateY = ROBOT_MOVING_FORWARD;
			speedRun = (joystickY, Y_THRESHOLD_HIGH, 255,
				MIN_PROGRAMMABLE_WHEEL_SPEED, MAX_PROGRAMMABLE_WHEEL_SPEED);
		}
		else {
			dirStateY = ROBOT_NOT_MOVING;
			speedRun = 0;
		}
	}
}

void joystickDrive() {
	mapJoystick();
	if (dirStateX == ROBOT_NOT_MOVING && dirStateY == ROBOT_NOT_MOVING) murahDrive.stop();
	else if (dirStateX == ROBOT_NOT_MOVING && dirStateY == ROBOT_MOVING_FORWARD) murahDrive.goForward(speedRun);
	else if (dirStateX == ROBOT_NOT_MOVING && dirStateY == ROBOT_MOVING_BACKWARD) murahDrive.goBackward(speedRun);
	else if (dirStateX == ROBOT_TURN_RIGHT && dirStateY == ROBOT_NOT_MOVING) murahDrive.turnRight(speedTurn, speedTurn);
	else if (dirStateX == ROBOT_TURN_LEFT && dirStateY == ROBOT_NOT_MOVING) murahDrive.turnLeft(speedTurn, speedTurn);
	else if (dirStateX == ROBOT_TURN_LEFT && dirStateY == ROBOT_MOVING_FORWARD) murahDrive.swayLeft(speedTurn, speedRun, false);
	else if (dirStateX == ROBOT_TURN_LEFT && dirStateY == ROBOT_MOVING_BACKWARD) murahDrive.swayLeft(speedTurn, speedRun, true);
	else if (dirStateX == ROBOT_TURN_RIGHT && dirStateY == ROBOT_MOVING_FORWARD) murahDrive.swayRight(speedTurn, speedRun, false);
	else if (dirStateX == ROBOT_TURN_RIGHT && dirStateY == ROBOT_MOVING_BACKWARD) murahDrive.swayRight(speedTurn, speedRun, true);
	else murahDrive.stop();

	//assess if needed, if NOT must remove in the future
	Serial.print("The robot's drive state: ");
	if(murahDrive.robotDriveState() != ROBOT_NOT_MOVING)
	Serial.println("MOVING");
	else Serial.println("NOT MOVING");

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
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

void loop()
{
	
	MurahBotSchedule.execute();
  
}
