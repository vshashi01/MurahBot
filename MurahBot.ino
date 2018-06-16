/*###############################################################################################################
 Project Name: MurahBot												   

 Current Version: 0.3.5												   


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



int speedTolerance = 30;
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


		murahDrive.stop(); //force stop the robot


		taskUpdateDriveState.disable();
	}
	
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


bool onEnableOfEnableDisableMurahBot() {
	taskEnableDisableMurahBot.setCallback(&callbackEnableDisableMurahBot);
	taskEnableDisableMurahBot.forceNextIteration();
	return true;
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

Drive4Wheel::DriveState dirStateX; // defined using Wheels Macros 
Drive4Wheel::DriveState dirStateY; // to keep robot wheel action state in 2 directions  

int speedRun; //forward backward speed 
int speedTurn; //turn speed 

BLYNK_WRITE(V1) {
	joystickX = param[0].asInt();
	joystickY = param[1].asInt();

	//Blynk input for joystick values 
}


void mapJoystick() {
	if (joystickX < X_THRESHOLD_LOW) {
		dirStateX = Drive4Wheel::DRIVE_LEFT;
		speedTurn = map(joystickX, X_THRESHOLD_LOW, 0,
			murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));

		if (joystickY < Y_THRESHOLD_LOW) {
			dirStateY = Drive4Wheel::DRIVE_BACKWARD_LEFT;
			speedRun = (joystickY, Y_THRESHOLD_LOW, 0,
				murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));
		}
		else if (joystickY > Y_THRESHOLD_HIGH) {
			dirStateY = Drive4Wheel::DRIVE_FORWARD;
			speedRun = (joystickY, Y_THRESHOLD_HIGH, 255,
				murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));

		}
		else {
			dirStateY = Drive4Wheel::DRIVE_STOP;
			speedRun = 0;
		}
	}
	else if (joystickX > X_THRESHOLD_HIGH) {
		dirStateX = Drive4Wheel::DRIVE_RIGHT;
		speedTurn = map(joystickX, X_THRESHOLD_HIGH, 255,
			murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));
		if (joystickY < Y_THRESHOLD_LOW) {
			dirStateY = Drive4Wheel::DRIVE_BACKWARD_RIGHT;
			speedRun = (joystickY, Y_THRESHOLD_LOW, 0,
				murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));
		}
		else if (joystickY > Y_THRESHOLD_HIGH) {
			dirStateY = Drive4Wheel::DRIVE_RIGHT;
			speedRun = (joystickY, Y_THRESHOLD_HIGH, 255,
				murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));
		}
		else {
			dirStateY = Drive4Wheel::DRIVE_STOP;
			speedRun = 0;
		}
	}
	else {
		dirStateX = Drive4Wheel::DRIVE_STOP;
		speedTurn = 0;
		if (joystickY < Y_THRESHOLD_LOW) {

			dirStateY = Drive4Wheel::DRIVE_BACKWARD;
			speedRun = (joystickY, Y_THRESHOLD_LOW, 0,
				murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));
		}
		else if (joystickY > Y_THRESHOLD_HIGH) {
			dirStateY = Drive4Wheel::DRIVE_FORWARD;
			speedRun = (joystickY, Y_THRESHOLD_HIGH, 255,
				murahDrive.getDriveSpeed(MIN), murahDrive.getDriveSpeed(MAX));
		}
		else {
			dirStateY = Drive4Wheel::DRIVE_STOP;
			speedRun = 0;


		}
	}
}

void joystickDrive() {
	mapJoystick();
	if (dirStateX == Drive4Wheel::DRIVE_STOP && dirStateY == Drive4Wheel::DRIVE_STOP) murahDrive.stop();


	else if (dirStateX == Drive4Wheel::DRIVE_STOP && dirStateY == Drive4Wheel::DRIVE_FORWARD) murahDrive.goForward(murahDrive.limitDriveSpeed(speedRun));
	else if (dirStateX == Drive4Wheel::DRIVE_STOP && dirStateY == Drive4Wheel::DRIVE_BACKWARD) murahDrive.goBackward(murahDrive.limitDriveSpeed(speedRun));
	else if (dirStateX == Drive4Wheel::DRIVE_RIGHT && dirStateY == Drive4Wheel::DRIVE_FORWARD) murahDrive.goRight(murahDrive.limitDriveSpeed(speedTurn), 1.0);
	else if (dirStateX == Drive4Wheel::DRIVE_LEFT && dirStateY == Drive4Wheel::DRIVE_FORWARD) murahDrive.goLeft(murahDrive.limitDriveSpeed(speedTurn), 1.0);
	//else if (dirStateX == DRIVE_LEFT && dirStateY == DRIVE_FORWARD) murahDrive.goLeft(speedTurn, 0.8, false);
	else if (dirStateX == Drive4Wheel::DRIVE_LEFT && dirStateY == Drive4Wheel::DRIVE_BACKWARD_LEFT) murahDrive.goLeft(murahDrive.limitDriveSpeed(speedTurn), 0.8, true);
	//else if (dirStateX == DRIVE_RIGHT && dirStateY == DRIVE_FORWARD) murahDrive.goRight(speedTurn, 0.8, false);
	else if (dirStateX == Drive4Wheel::DRIVE_RIGHT && dirStateY == Drive4Wheel::DRIVE_BACKWARD_RIGHT) murahDrive.goRight(murahDrive.limitDriveSpeed(speedTurn), 0.8, true);


	else murahDrive.stop();

	//assess if needed, if NOT must remove in the future
	Serial.print("The robot's drive state: ");
	if(murahDrive.getCurrentDriveState() != Drive4Wheel::DRIVE_STOP)	Serial.println("MOVING");
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
