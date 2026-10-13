/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       student                                                   */
/*    Created:      10/9/2026    10-12-26                                                */
/*    Description:  VEX V5 Standard Drivebase                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "vex.h"

using namespace vex;

// Competition object
competition Competition;

// Controller
controller Controller;

// Drive motors
motor LeftFront(PORT1, gearSetting::ratio18_1, false);
motor LeftBack(PORT2, gearSetting::ratio18_1, false);

motor RightFront(PORT9, gearSetting::ratio18_1, true);
motor RightBack(PORT10, gearSetting::ratio18_1, true);

// Motor groups
motor_group LeftDrive(LeftFront, LeftBack);
motor_group RightDrive(RightFront, RightBack);

// Drive speed
int driveSpeed = 100;

//Inchdrive
void inchDrive(double inches, int speed) {


// Convert inches to motor degrees
double wheelCircumference = 2.75 * 3.14159265358979323;
double motorDegrees = (inches / wheelCircumference) * 360;

// Set motor speed
LeftDrive.setVelocity(speed, percent);
RightDrive.setVelocity(speed, percent);

// Drive forward
LeftDrive.spinFor(forward, motorDegrees, degrees, false);
RightDrive.spinFor(forward, motorDegrees, degrees, true);


}

// Pre-Autonomous
void pre_auton(void) {
LeftDrive.setStopping(brakeType::coast);
RightDrive.setStopping(brakeType::coast);

LeftDrive.setVelocity(driveSpeed, percent);
RightDrive.setVelocity(driveSpeed, percent);
}

// Autonomous
void autonomous(void) {

}

// Driver Control
void usercontrol(void) {
while (true) {
// Right-stick arcade drive:
// Axis2 controls forward/reverse,
// Axis1 controls turning.


int forwardPower = Controller.Axis2.position();
int turnPower = Controller.Axis1.position();

// Optional joystick deadband
if (abs(forwardPower) < 5) {
  forwardPower = 0;
}

if (abs(turnPower) < 5) {
  turnPower = 0;
}

// Calculate left and right motor power
int leftPower = forwardPower + turnPower;
int rightPower = forwardPower - turnPower;

// Limit motor power to -100 through 100
if (leftPower > 100) leftPower = 100;
if (leftPower < -100) leftPower = -100;

if (rightPower > 100) rightPower = 100;
if (rightPower < -100) rightPower = -100;

// Control left drive
LeftDrive.setVelocity(abs(leftPower), percent);

if (leftPower > 0) {
  LeftDrive.spin(forward);
} else if (leftPower < 0) {
  LeftDrive.spin(reverse);
} else {
  LeftDrive.stop();
}

// Control right drive
RightDrive.setVelocity(abs(rightPower), percent);

if (rightPower > 0) {
  RightDrive.spin(forward);
} else if (rightPower < 0) {
  RightDrive.spin(reverse);
} else {
  RightDrive.stop();
}

wait(20, msec);


}
}

// Main
int main() {
Competition.autonomous(autonomous);
Competition.drivercontrol(usercontrol);

pre_auton();

while (true) {
wait(100, msec);
}
}
