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


#include <Bounce2.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskScheduler.h>
#include "Wheels.h"
#include <Arduino.h>



///////////////////////////////////////////////////////////////////////////
// Wheel class and Drive4Wheel class instantiation 
Wheel* WheelFrontLeft = new Wheel(46, 47, 5);
Wheel* WheelFrontRight = new Wheel(48, 49, 4);
Wheel* WheelRearLeft = new Wheel(50, 51, 7);
Wheel* WheelRearRight = new Wheel(52, 53, 6);

Drive4Wheel myRobot(WheelFrontLeft, WheelFrontRight,
	WheelRearLeft, WheelRearRight);

////////////////////////////////////////////////////////////////////////////////
// Button Pin initialization and Bounce class instantiation 
const byte buttonPinForward = 22;
const byte buttonPinBackward = 23;
const byte buttonPinLeft = 24;
const byte buttonPinRight = 25;

Bounce BounceForward(buttonPinForward, 10);
Bounce BounceBackward(buttonPinBackward, 10);
Bounce BounceLeft(buttonPinLeft, 10);
Bounce BounceRight(buttonPinRight, 10);


///////////////////////////////////////////////////////////////////////////////
// Global Variable Initialization 
int drivingCondition = 0; //updated for different button presses




///////////////////////////////////////////////////////////////////////////////
// Scheduler and Tasks instantiation
Scheduler MurahBotSchedule;
Task updateButton(40, TASK_FOREVER, &callbackButtonState, &MurahBotSchedule);
Task updateDriveState(50, TASK_FOREVER, &callbackDrive, &MurahBotSchedule);

////////////////////////////////////////////////////////////////////////////////
// updates drivingCondition based on the button press 
void callbackButtonState() {
	if (BounceForward.update()) {
		if (BounceForward.read() == HIGH) {
			if (drivingCondition != 0) {
				drivingCondition = 0;
				Serial.println("Stopping..");
			}
			else {
					drivingCondition = 1;
					Serial.println("Moving Forward..");				
			}
		}
	}

	else if (BounceBackward.update()) {
		if (BounceBackward.read() == HIGH) drivingCondition = 2;
	}
	else if (BounceLeft.update()) {
		if (BounceLeft.read() == HIGH) drivingCondition = 3;
	}
	else if (BounceRight.update()) {
		if (BounceRight.read() == HIGH) drivingCondition = 4;
	}
	else;


	updateButton.setCallback(&callbackButtonState);
}

// updates the robot DriveState based on the drivingCondition variable 
void callbackDrive() {
	if (updateDriveState.isFirstIteration()) {
		Serial.println("Masuk Once");
		updateDriveState.setCallback(callbackDrive);
		updateDriveState.delay(5000);
	}
	else {
		if (drivingCondition != 0) {
			if (drivingCondition == 1) {
				myRobot.goForward();			
			}
			else if (drivingCondition == 2) {
				myRobot.goBackward();				
			}
			else if (drivingCondition == 3) {
				myRobot.turnLeft();				
			}
			else if (drivingCondition == 4) {
				myRobot.turnRight();
			}


			else;
		}
		else {
			myRobot.stop();
		}

		updateDriveState.setCallback(&callbackPrintState);
	}
}

//prints drivestate ONLY FOR DEBUG PURPOSES
void callbackPrintState() {
	Serial.print("The robot is ");
	if (myRobot.robotDriveState() != ROBOT_NOT_MOVING) Serial.println("moving.");
	else Serial.println("not moving.");

	updateDriveState.setCallback(&callbackDrive);

}

void setup()
{
	pinMode(buttonPinForward, INPUT_PULLUP);
	pinMode(buttonPinBackward, INPUT_PULLUP);
	pinMode(buttonPinLeft, INPUT_PULLUP);
	pinMode(buttonPinRight, INPUT_PULLUP);
	Serial.begin(9600);
	delay(1000);


	updateDriveState.enable();
	updateButton.enable();


  /* add setup code here */

}

void loop()
{
	MurahBotSchedule.execute();
  
}
