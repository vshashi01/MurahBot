# MurahBot
App Controlled 4-Wheeled Mobile Robot through Blutooth Low Energy(BLE) module with Arduino Mega2560 board. 

## Aim of the Project 
I was once a very ambitious FYP student who wants to explore Mobile Robotics. However, I quickly realised that although the internet 
has alot of guides and forums that could help to start such prototyping with Arduino/Raspberry Pi, they are only catered towards simple projects for beginners. 
As the design requirements and complexity of the project increases there is little guide to assist them/us.

This project aims to be a: 
  1. Quick start for anyone who needs to quickly get things up and running with a MobileRobot 
  2. Very cheap and basic bluetooth controlled MobileRobot to be built upon by anyone.
  3. Modularly built for varied users for prototypers. 
  
 ### When would the MurahBot would have achieved its goal?
 
 **_MurahBot would be considered complete when the basic Kinematic capabilites that is available on a TurtleBot along with a ROS Interface has been made possible to the MurahBot._**
 
 _This ReadME is not meant as a step by step tutorial. However, if you understand basic Arduino programming and some C++ concepts, coupled with the comments in the codes
 you will be able to understand and adopt the files for your personal uses._
 
 _Having said that, I will present some basic design decisions and intermediate concepts pertaining to good microprocessor programs,
 such as Use of Existing libraries like Task Schedulers, Bouncy for PushButton Debouncing and my custom made Wheels library_
 
 ## MurahBot 0.3.6
 I have filed many items with GitHub issues for future improvements I have in mind in the MurahBot pipeline. For now I will be sharing the version 0.3.6 which has a few features 
  
  1. **Using Cooperative MultiTasking Approach for the programs with Arkhipenko's Task Scheduler Library for Arduino**
  
  2. **Yet another library for controlling Wheels using Motor Drivers with my own Wheels library**
  
  3. **Use of the Blynk App and the Bluetooth 4.0 Module to control the Mobile Robot with a Joystick Like Input**
  
  4. **Use of Push Button Matrix as proof of Concept for a Completely Reactive Mobile Robot System**
  
 THis will not be the last version of the library. As can be seen in the issue list, there are many enhancement and improvements ideas are in the pipeline. 
 However, currently I am shifting my focus to redevelop the project in different development platform to explore its advantages.
 This requires reimplementation existing sources with new libraries and new implementations. This may or may not be a good idea but I am going to do it anyway.  

## Some design decisions

### Mutltitasking the Robot with Task Scheduler Library 
One of the limitations that no one talks about in the begineer tutorials, is the fact that the the basic procedural programming structure running within 
the void loop() is not nearly as versatile as we think it is. Procedural programmings are often very reactive and requires something to happen on specified time frame 
to be able to be performed. 

The work around for this is to use a Real Time OS or a Multitasking Approach to the program. 
While researching I found a very complete library by Arkhipenko called [Task Scheduler for Arduino Microcontrollers](https://github.com/arkhipenko/TaskScheduler). Reading the examples and the guide provided by Arkhipenko could be really helpful. 
With Cooperative Multitasking implementations, we are able to break our programs into smaller chunks that are run based on the interval set by us. With this we can make sure that not one of our reactions will hog the system. 
Reading up more on the Cooperative Multitasking could be very helpful. This is a very useful [Stack Overflow answer](https://stackoverflow.com/questions/24128864/how-to-achieve-multitasking-in-a-microcontroller)

### Using 2 L298N Motor Driver for 4 Wheels
Although I am aware that this motor driver could be a power hog within a battery based systems. It is the cheapest available for prototypers that is easily accesible from online shops or robotic shops. 
The dirt cheap price of L298N motor driver is the main reason I chose it. 

Essentially 1 Driver can control two different motors since its a Dual H-bridge driver. In fact many online tutorials will show you how you can share 1 driver for 4-Wheeled [Differential Drive Robots](https://en.wikipedia.org/wiki/Differential_wheeled_robot).  
However, using 1 driver to control the 4 wheels would result in an even more imbalanced kinematics of the mobile robot than using 2 different drivers for 4 wheels since the current supplied to the DC motors will continue to fluctuate more wildly due the circuit node sharing.
So I went with 2 drivers, each wheels with its own motor and own control pins. 

### Use of Bluetooth 4.0 module over conventional Bluetooh 2.0
Since I was essentially testing my device from an iOS device, using Bluetooth modules such HC-05 that runs on Bluetooth 2.0 simply wouldnt work. 
So I made conscious decision to use the JDY-08 Bluetooth 4.0 module as the connectivity module for the MurahBot. However, with some benchmarks, I realised that BLE modules are even more effective since they use less energy 
and has a good idle/sleep protocal to save energy. Sending lightweight data's was essentially much more faster and quicker with BLE protocol. More read up on the differences could help you to choose the suitable module. 

Also, since I used the **Arduino Mega2560**, I could get away with using the extra Hardware Serial ports available on my board. This could be an important point since I am not sure about the limitations that you wuld face if you choose to implement the BLE connection with the existing Software Serial library. 

*Note: I did attempt to use the Software Serial library in my Multitasking project at first and it simply didnt work so that might be another reason why I think getting the Mega2560 would not be such a bad idea.*


### Using Blynk App and APIs to interface with the Robot 
As soon as I started looking at interfacing from an App running on a mobile device, I realised that most solutions/tutorials that were available On the Net uses the slow and heavy Serial communication protocal with strings. 
Using clone bluetooth modules could make this even trickier since these clones usually have different ways of concatenating or appending strings upon sending/receiving the data.  

I needed a more robust solution such as the Blynk framework that provides various APIs for interfacing IoT and microcontroller devices with the Internet/Cloud or through wireless protocols with mobile app in iOS or Android platforms. 
Reading up on [Blynk app and framework](https://www.blynk.cc/) could help. 

### Pushbuttons as a prove of concept 
With a 4x1 pushbutton matrix, I wanted to use it as a proof of concept for prototypers who wants some market like button functions on their mobile robot. 
Currently the program uses only one of the 4 push buttons as a System Start and Stop button. Using Push buttons in an event based program could also result in some quirks when the button is not debounced properly. 
Especially cheap mechanical push buttons come with high intervals of mechanical debounce,as such we need a software level debouncing method to work with Push Button easily. 
Enter the [Bounce 2 Arduino library](https://github.com/thomasfredericks/Bounce2) by Thomas Fredericks. There are some quirks in this sketch that also shows some workaround for some limitations that a software level debouncing has. 

*Hint: If you choose to use push button extensively, make sure you run enough tests on using these buttons with various conditions.*

### Writing my OWN Wheels library 
I am aware and strongly believe in the philosophy to not reinvet the wheel everytime we try to implement a program or software solution. 
However, I also realised that even if I used existing solutions for the Drive systems that are available in the internet I would still need to be pervasive of their implementation of the library to suit my design pattern.
I designed the Wheels library with Wheel Class and Drive4Wheel Class with the idea of expanding it with a built in Kinematics and Dynamic Manager Class which is **still in progress**.

Although I have only tested the library with L298N Motor Driver, essentially the library would work with any DC motor driver that has 3-pin configuration (2 for Poles and 1 to set speed). 
Also, if the resolution of the motordriver is more than just the 8-bit integer, you can tweak the maximum Wheel Absolute Speeds by changing the values in the constructor of the Wheel class for variables minAbsoluteWheelSpeed and maxAbsoluteWheelSpeed. 

I have also used extensive pointers across the implementation to suit my design pattern. IF you as a user is against the use of raw Pointers and dependent classes, then feel free to ditch the library for one that suits you. 
That would come at a cost since, you would have to reconfigure the **callbackPrimaryJoystickDrive()** and **callbackSecondaryJoystickDrive()** functions with the appropriate drive methods available in the new implementation. 
Also, I implemented and used Getter and Setter methods as the way for the owners to access the state of the Wheels and the Drive4Wheel class to ensure external corruption of the State Variables do not happen. 
This may also come as undesrible to some users who belives such overhed are not needed in their programs. 

### Using Arduino Mega2650
The primary reason why I used the Mega2560 is because I had one. But as I began to develop the project, I quickly realised that this project would need alot of GPIO's. Currently I make use of approximately 16 GPIO's *(if I am not mistaken)* anbd more to be used in the future for various sensors I plan to add to MurahBot.
Another advantage of the project was that Mega2560 has extra Serial ports to be used to connect the BLE module. 

**If you managed to port this program and make it work on other Arduino boards, feel free to create a new branch in the repository and work on it**

### Using Visual Micro with Visual Studio 2017 for Development 
Many people would want to develop the program on the basic Arduino IDE. I am not a fan of it. It works as long as the project is small and you dont have to debug your own library. 
Since [VS2017 Community](https://visualstudio.microsoft.com/downloads/) is a free version, any hobbyist or student developer can use it for their own problem. [Visual Micro](https://www.visualmicro.com/) too is very powerful with Debugging tools for your development. 

## What's next?
Currently I am confilcited with many parallel ideas and goals for the MurahBot **(cry for Colloborators to Spar and work with)**. 

  - Porting the current code base to PlatformIO for a more robust implementation of MurahBot
  - Adding a second microprocessor to support Preemptive Interrupt Routines for Cheap IR Encoders for Wheel Kinematics and Ultrasonic Sensors for Obstacle Detection 
  - Weighing the possibility of adopting a fully C++ like implementation for MurahBot rather than the 'C with More Features Approach'
  - Weighing the possibility of adopting FreeRTOS scheduler and ditching Arduino only Task Scheduler Library (Maybe a bad Idea?)

*Any feedbacks for these many ideas are also welcomed*

Due to other responsibilites, my efforts here is slowing. I hope to update this repository as often as possible. 
For now MurahBot 0.3.6 will be the version that I am convinced could be a good launching for anyone who needs a cheap MobileRobot. Based on the Issues setup previously if you managed to develop solutions that could be added to the MurahBot and would like to share it, feel free to branch out and work them. 
For any more queries feel free to email me to **vshashi01@gmail.com** or create an Issue and tag me **@vshashi01**

**I am not essentially a software developer so any comments and feedbacks about the code quality is very welcomed :)**

**_Kudos..._**
