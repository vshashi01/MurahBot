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


//#include <Blynk.h>
//#include "NewPing.h"

#include <Bounce2.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskScheduler.h>
#include "Wheels.h"
#include <Arduino.h>
#include <TimerThree.h>




Wheel FrontLeftWheel(46, 47, 5);
Wheel FrontRightWheel(48, 49, 4);
Wheel RearLeftWheel(50, 51, 7);
Wheel RearRightWheel(52, 53, 6);

Drive4Wheel myRobot(FrontLeftWheel, FrontRightWheel,
	RearLeftWheel, RearRightWheel);

//const byte frontLeftEncoder = 3;
//const byte frontRightEncoder = 2;
//const byte rearLeftEncoder = 19;
//const byte rearRightEncoder = 18;

const byte buttonPinForward = 22;
const byte buttonPinBackward = 23;
const byte buttonPinLeft = 24;
const byte buttonPinRight = 25;


Bounce bounceForward(buttonPinForward, 10);
Bounce bounceBackward(buttonPinBackward, 10);
Bounce bounceLeft(buttonPinLeft, 10);
Bounce bounceRight(buttonPinRight, 10);

int drivingCondition = 0;
int testButton = 0;
int currentButtonState = 0;

bool myRobotMoving = false;

Scheduler myRobotSchedule;
Task updateButton(40, TASK_FOREVER, &callbackButtonState, &myRobotSchedule);
Task updateDriveState(50, TASK_FOREVER, &callbackDrive, &myRobotSchedule);

void callbackButtonState() {
	if (bounceForward.update()) {
		if (bounceForward.read() == HIGH) {
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

	else if (bounceBackward.update()) {
		if (bounceBackward.read() == HIGH) drivingCondition = 2;
	}
	else if (bounceLeft.update()) {
		if (bounceLeft.read() == HIGH) drivingCondition = 3;
	}
	else if (bounceRight.update()) {
		if (bounceRight.read() == HIGH) drivingCondition = 4;
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
	if (myRobotMoving) Serial.println("moving.");
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
