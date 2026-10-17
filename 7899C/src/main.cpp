
#include "vex.h"

using namespace vex;

// Competition
competition Competition;

// Brain and controller
brain Brain;
controller Controller1;

// Drive motors
motor LeftFront(PORT10, ratio18_1, true);
motor LeftBack(PORT20, ratio18_1, true);
motor RightFront(PORT1, ratio18_1, false);
motor RightBack(PORT11, ratio18_1, false);

motor_group LeftDrive(LeftFront, LeftBack);
motor_group RightDrive(RightFront, RightBack);

// Intake
motor Intake(PORT5, ratio18_1, false);

// Arm
motor Arm(PORT8, ratio18_1, false);

// Claw
motor Claw(PORT9, ratio18_1, false);

// *** CHANGED: Use the actual port of your inertial sensor ***
// PORT12 is an example. Change if your sensor uses another port.
inertial Inertial1(PORT12);

// --------------------------------------------------
// Limit motor power
// --------------------------------------------------

int limitPower(int power)
{
  if (power > 100)
    return 100;

  if (power < -100)
    return -100;

  return power;
}

// --------------------------------------------------
// Motor health display
// --------------------------------------------------

void printMotorStatus(const char* name, motor& m)
{
  if (!m.installed())
  {
    Brain.Screen.print("%s: NOT CONNECTED\n", name);
    return;
  }

  double current = m.current(amp);
  double temp = m.temperature(celsius);

  Brain.Screen.print(
      "%s: %.1fA  %.1fC\n", name, current, temp);
}

void Display()
{
  Brain.Screen.clearScreen();
  Brain.Screen.print("VEX V5 MOTOR HEALTH\n\n");

  printMotorStatus("LeftFront", LeftFront);
  printMotorStatus("LeftBack", LeftBack);
  printMotorStatus("RightFront", RightFront);
  printMotorStatus("RightBack", RightBack);
  printMotorStatus("Intake", Intake);
  printMotorStatus("Arm", Arm);
  printMotorStatus("Claw", Claw);
}

// --------------------------------------------------
// Pre-autonomous
// --------------------------------------------------

void pre_auton(void)
{
  Brain.Screen.clearScreen();
  Brain.Screen.print("Initializing robot...\n");

  LeftDrive.setStopping(brake);
  RightDrive.setStopping(brake);

  Intake.setStopping(coast);
  Arm.setStopping(hold);
  Claw.setStopping(hold);

  // Torque limits
  LeftFront.setMaxTorque(100, percent);
  LeftBack.setMaxTorque(100, percent);
  RightFront.setMaxTorque(100, percent);
  RightBack.setMaxTorque(100, percent);

  Intake.setMaxTorque(70, percent);
  Arm.setMaxTorque(60, percent);
  Claw.setMaxTorque(50, percent);

  // Calibrate inertial sensor
  Inertial1.calibrate();

  int timeout = 0;

  while (Inertial1.isCalibrating() && timeout < 5000)
  {
    wait(50, msec);
    timeout += 50;
  }

  Brain.Screen.clearScreen();

  if (Inertial1.isCalibrating())
  {
    Brain.Screen.print(
        "Inertial calibration timeout!\nCheck sensor connection.");
  }
  else
  {
    Brain.Screen.print("Pre-auton complete!");
  }

  wait(1, sec);
}

// --------------------------------------------------
// Autonomous
// --------------------------------------------------

void autonomous(void)
{
  Arm.spin(reverse, 40, percent);

  wait(1, sec);

  Arm.stop(hold);

  LeftDrive.spin(forward, 22, percent);
  RightDrive.spin(forward, 22, percent);

  wait(1, sec);

  LeftDrive.stop(brake);
  RightDrive.stop(brake);

  wait(0.5, sec);

  LeftDrive.spin(reverse, 11, percent);
  RightDrive.spin(forward, 11, percent);

  wait(1.2, sec);

  LeftDrive.stop(brake);
  RightDrive.stop(brake);

  wait(0.2, sec);

  LeftDrive.spin(forward, 22, percent);
  RightDrive.spin(forward, 22, percent);
  
  wait(1, sec);
   LeftDrive.stop(brake);
   RightDrive.stop(brake);

   wait(0.2, sec);
   Arm.stop(brake);
   wait(0.1,sec);
   Arm.spin(forward, 10, percent);
   wait(0.5, sec);
   Arm.stop(brake);

}

// --------------------------------------------------
// Driver control
// --------------------------------------------------

void usercontrol(void)
{
  int displayCounter = 0;

  while (true)
  {
    // ARCADE DRIVE
    float speed = 0.5;
    int forwardVal = Controller1.Axis3.position() * speed;
    int turnVal = Controller1.Axis1.position() * speed;

    // Deadband
    if (forwardVal > -5 && forwardVal < 5)
      forwardVal = 0;

    if (turnVal > -5 && turnVal < 5)
      turnVal = 0;

    int leftPower = forwardVal + turnVal;
    int rightPower = forwardVal - turnVal;

    // *** CHANGED: Straight-line drift correction ***
    // Reduce left power slightly when not turning.
    // Try 0.95 first; adjust after testing.
    if (turnVal == 0)
    {
      leftPower = (int)(leftPower * 0.85);
    }

    // Limit motor power
    leftPower = limitPower(leftPower);
    rightPower = limitPower(rightPower);

    LeftDrive.spin(forward, leftPower, percent);
    RightDrive.spin(forward, rightPower, percent);

    // ARM: L1 raises, L2 lowers
    if (Controller1.ButtonR1.pressing())
    {
      Arm.spin(forward, 40, percent);
    }
    else if (Controller1.ButtonR2.pressing())
    {
      Arm.spin(reverse, 40, percent);
    }
    else
    {
      Arm.stop(hold);
    }

    // CLAW: A closes, B opens
    if (Controller1.ButtonL2.pressing())
    {
      Claw.spin(forward, 30, percent);
    }
    else if (Controller1.ButtonL1.pressing())
    {
      Claw.spin(reverse, 30, percent);
    }
    else
    {
      Claw.stop(hold);
    }

    // INTAKE: R1 forward, R2 reverse
    if (Controller1.ButtonR1.pressing())
    {
      Intake.spin(forward, 70, percent);
    }
    else if (Controller1.ButtonR2.pressing())
    {
      Intake.spin(reverse, 70, percent);
    }
    else
    {
      Intake.stop(coast);
    }

    // Update health display approximately every 500 ms
    displayCounter++;

    if (displayCounter >= 25)
    {
      Display();
      displayCounter = 0;
    }

    wait(20, msec);
  }
}

// --------------------------------------------------
// Main
// --------------------------------------------------

int main()
{
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  pre_auton();

  while (true)
  {
    wait(100, msec);
  }
}
