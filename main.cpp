#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>

// Motors
FEHMotor left_motor(FEHMotor::Motor1, 12.0);
FEHMotor right_motor(FEHMotor::Motor3, 12.0);
FEHServo arm(FEHServo::Servo0);

//Encoders
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_7);


// Bump Switches
/*
DigitalInputPin front_left(FEHIO::);
DigitalInputPin front_right(FEHIO::);
DigitalInputPin back_left(FEHIO::);
DigitalInputPin back_right(FEHIO::);
*/

AnalogInputPin cds_cell(FEHIO::P2_7); // CdS Cell

// Optosensors
/*
AnalogInputPin left_sensor(FEHIO::);
AnalogInputPin middle_sensor(FEHIO::);
AnalogInputPin right_sensor(FEHIO::);

const double left_threshold = ;
const double middle_threshold = ;
const double right_threshold = ;
*/


const int sleep_time = 200; //The amount of time to sleep between functions.
const int right_turn_counts = 235; //Expected encoder counts for the right IGWAN motor.
const int left_turn_counts = 225; //Expected encoder counts for the left IGWAN motor.
const int motor_left_percent = 25; //Motor Speed percentage for the left motor.
const int motor_right_percent = 25; //Motor Speed percentage for the right motor.
const int percent = 25; //General motor percentage.

bool touch_start() // Wait for screen touch
{
    float x, y;
    bool temp = false;
    if(LCD.Touch(&x, &y))
    {
        temp = true;
    }
    return temp;
}

const double red_light = 0.3; // max value
const double blue_light = 0.7; // max value

  // Detects the starting red light, wait and returns true when detected.
bool light_start()
{
    bool start = false;
    if (cds_cell.Value() < red_light)
    {
        start = true;
    }
    return start;
}

  // Detects whether the light is red or blue. Returns a string describing the detected light or a default case.
int light_red_or_blue()
{
    int light = 0;
    if(cds_cell.Value() < 0.2)
    {
        LCD.WriteLine("Light is red."); // return 1
        light = 1;
    }
    else if(cds_cell.Value() < 0.7)
    {
        LCD.WriteLine("Light is blue."); // return 2
        light = 2;
    }
    else
    {
        LCD.WriteLine("No light is detected."); // return 0;
    }
    return light;
}


  // Drive forward a set distance
void drive(int motor_left_percent, int motor_right_percent, int counts)
{
      // Reset the left and right encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

      // Set the right motor to positive right percentage value, and the left motor to the positive left percentage value.
    right_motor.SetPercent(motor_right_percent);
    left_motor.SetPercent(motor_left_percent);

      // While the average value of the left and right encoder counts is less than the expected count value, keep running both motors.
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

      // Turn both motors off.
    right_motor.Stop();
    left_motor.Stop();

    Sleep(sleep_time);
}


  // Turn left a set amount based on the encoder counts
void turn_left(int motor_left_percent, int motor_right_percent, int counts)
{
      // Reset the left and right encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

      // Set the right motor to positive right percentage value, and the left motor to the negative left percentage value.
    right_motor.SetPercent(motor_right_percent);
    left_motor.SetPercent(-motor_left_percent);

      // While the average value of the left and right encoder counts is less than the expected count value, keep running both motors.
    while((left_encoder.Counts() + right_encoder.Counts()) / 2 < counts);

      // Turn both motors off.
    right_motor.Stop();
    left_motor.Stop();

    Sleep(sleep_time);
}


  // Turn right a set amount based on the encoder counts
void turn_right(int motor_left_percent, int motor_right_percent, int counts) // using encoders
{
      // Reset the left and right encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

      // Set the right motor to negative right percentage value, and the left motor to the positive left percentage value.
    right_motor.SetPercent(-motor_right_percent);
    left_motor.SetPercent(motor_left_percent);

      // While the average value of the left and right encoder counts is less than the expected count value, keep running both motors.
    while((left_encoder.Counts() + right_encoder.Counts()) / 2 < counts);

    // Turn both motors off.
    right_motor.Stop();
    left_motor.Stop();

    Sleep(sleep_time);
}

// Prints out the left and ride encoder counts
void test_encoder_counts()
{
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");
}

int main() {
    LCD.Clear(FEHLCD::Black);
    LCD.SetFontColor(FEHLCD::White);

    // Set the min and max values for the arm's servo motor.
    arm.SetMin(500);
    arm.SetMax(2480);

    // Start a timer and enter a loop. Call the light_start() method. When that method returns true or 20 seconds have passed, end the loop.
    int start_time = TimeNow();
    while(!light_start() && TimeNow() - start_time < 20.0);


    // Drive out of the starting area and go forward 10 inches.
    drive(motor_left_percent, motor_right_percent, 405);
    test_encoder_counts();

    // Turn left towards the ramp
    turn_left(motor_left_percent, motor_right_percent, left_turn_counts);
    test_encoder_counts();

    // Drive towards the ramp, going forward 12 inches.
    drive(motor_left_percent, motor_right_percent, 486);
    test_encoder_counts();

    // Turn left to face the ramp
    turn_left(motor_left_percent, motor_right_percent, left_turn_counts);
    test_encoder_counts();

    // Drive up the ramp, going forward 30 inches.
    drive(motor_left_percent + 1, motor_right_percent, 1215);
    test_encoder_counts();

    // Turn right towards the wall
    turn_right(motor_left_percent, motor_right_percent, right_turn_counts);
    test_encoder_counts();

    // Drive towards the wall, going forward 8 inches.
    drive(motor_left_percent, motor_right_percent, 324);
    test_encoder_counts();

    // Turn right, back faces the button
    turn_right(motor_left_percent, motor_right_percent, right_turn_counts);
    test_encoder_counts();

    // Back into the button, going backwards 10 inches.
    drive(-motor_left_percent, -motor_right_percent, 810);
    test_encoder_counts();
    Sleep(8000);

    // Drive forwards from the button, going forward 16 inches.
    drive(motor_left_percent, motor_right_percent, 648);
    test_encoder_counts();

    // Turn right, facing the lever
    turn_right(motor_left_percent, motor_right_percent, right_turn_counts);
    test_encoder_counts();

    // Drive forwards towards the lever, forward 15 inches.
    drive(motor_left_percent, motor_right_percent, 608);
    test_encoder_counts();

    return 0;
}
