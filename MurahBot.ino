/*##########################################################################
# Project Name: MurahBot												   #
# Current Version: 0.2.0												   #
# Description: A 4WD mobile robot made of 4 motors, 2 Dual H Bridge Motor  #
#			   Drivers, with Arduino Mega2560 connected to HC05 Bluetooth  #
#			   Module with Blynk framework.								   #
# Developer: Shashi														   #
# Email: vshashi01gmail.com												   #
# GitHub:																   #
# External Libraries used:												   #
#	1. Blynk															   #
#	2. Wheels library (self-made)										   #
#	3. Task Schedular Library											   #
#																		   #
###########################################################################*/


#include <Bounce2.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskScheduler.h>
#include "Wheels.h"
#include <Arduino.h>
#include <TimerThree.h>




Wheel* WheelFrontLeft = new Wheel(46, 47, 5);
Wheel* WheelFrontRight = new Wheel(48, 49, 4);
Wheel* WheelRearLeft = new Wheel(50, 51, 7);
Wheel* WheelRearRight = new Wheel(52, 53, 6);

Drive4Wheel myRobot(WheelFrontLeft, WheelFrontRight,
	WheelRearLeft, WheelRearRight);

//const byte frontLeftEncoder = 3;
//const byte frontRightEncoder = 2;
//const byte rearLeftEncoder = 19;
//const byte rearRightEncoder = 18;

const byte buttonPinForward = 22;
const byte buttonPinBackward = 23;
const byte buttonPinLeft = 24;
const byte buttonPinRight = 25;


Bounce BounceForward(buttonPinForward, 10);
Bounce BounceBackward(buttonPinBackward, 10);
Bounce BounceLeft(buttonPinLeft, 10);
Bounce BounceRight(buttonPinRight, 10);

int drivingCondition = 0;
int testButton = 0;
int currentButtonState = 0;

bool myRobotMoving = false;

Scheduler myRobotSchedule;
Task updateButton(40, TASK_FOREVER, &callbackButtonState, &myRobotSchedule);
Task updateDriveState(50, TASK_FOREVER, &callbackDrive, &myRobotSchedule);

void callbackButtonState() {
	if (BounceForward.update()) {
		if (BounceForward.read() == HIGH) {
			if (drivingCondition != 0) {
				drivingCondition = 0;
				Serial.println("Stopping..");
			}
			else {
				if (testButton == 0) { // to correct some flaws in the button used. Test in future.
					testButton = 1;
					Serial.println("Should Come out maximum once");
				}
				else {
					drivingCondition = 1;
					Serial.println("Moving Forward..");
				}
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
				myRobotMoving = true;
			}
			else if (drivingCondition == 2) {
				myRobot.goBackward();
				myRobotMoving = true;
			}
			else if (drivingCondition == 3) {
				myRobot.turnLeft();
				myRobotMoving = true;
			}
			else if (drivingCondition == 4) {
				myRobot.turnRight();
				myRobotMoving = true;
			}


			else;
		}
		else {
			myRobot.stop();
			myRobotMoving = false;
		}

		updateDriveState.setCallback(&callbackPrintState);
	}
}

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
	myRobotSchedule.execute();
  /* add main program code here */

}
