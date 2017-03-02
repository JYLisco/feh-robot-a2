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

const int motor_left_percent = 26; //Motor Speed percentage for the left motor.
const int motor_right_percent = 25; //Motor Speed percentage for the right motor.

const int arm_Min = 500; // Servo min, for calibration purposes
const int arm_Max = 2480; // Servo max, for calibration purposes
const int default_degree = 90; // Default degree servo should be at


// Encoders
DigitalEncoder right_encoder(FEHIO::P0_7);
DigitalEncoder left_encoder(FEHIO::P2_7);

const int encoder_inch = 40; // Number of encoder counts for a single inch.


// Bump Switches
/*
DigitalInputPin front_left(FEHIO::);
DigitalInputPin front_right(FEHIO::);
DigitalInputPin back_left(FEHIO::);
DigitalInputPin back_right(FEHIO::);
*/

AnalogInputPin cds_cell(FEHIO::P1_6); // CdS Cell

const double red_light = 0.3; // max value
const double blue_light = 0.7; // max value

// Optosensors
/*
AnalogInputPin left_sensor(FEHIO::);
AnalogInputPin middle_sensor(FEHIO::);
AnalogInputPin right_sensor(FEHIO::);

const double left_threshold = ;
const double middle_threshold = ;
const double right_threshold = ;
*/


const int sleep_time = 200; // The amount of time to sleep between functions.
const int right_turn_counts = 235; // Expected encoder counts for the right IGWAN motor.
const int left_turn_counts = 228; // Expected encoder counts for the left IGWAN motor.


// Prints out the left and ride encoder counts
void test_encoder_counts()
{
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");
}


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
    if(cds_cell.Value() < 0.3)
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

    test_encoder_counts();
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

    test_encoder_counts();
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

    test_encoder_counts();
    Sleep(sleep_time);
}



void servo_calibrate()
{
    // Around 90 degrees seems like a good starting point
    int degree = default_degree;
    arm.SetDegree(degree);

    float x, y;
    while(true)
    {
        if(LCD.Touch(&x, &y))
        {
            if(x < 160)
            {
                degree -= 5;
                arm.SetDegree(degree);

            }
            else
            {
                degree += 5;
                arm.SetDegree(degree);
            }
            LCD.Write("degree: ");
            LCD.WriteLine(degree);
            LCD.Write("x: ");
            LCD.WriteLine(x);
        }
        Sleep(200);

    }
}

void check_x_plus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, 20);
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, 20);
        }
    }
}

void check_x_minus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, 20);
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, 20);
        }
    }
}

void check_y_minus(float y_coordinate) //using RPS while robot is in the -y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, 20);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, 20);
        }
    }
}

void check_y_plus(float y_coordinate) //using RPS while robot is in the +y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, 20);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, 20);
        }
    }
}


void check_heading(float heading) //using RPS
{
    //you will need to fill out this one yourself and take into account
    //the edge conditions (when you want the robot to go to 0 degrees
    //or close to 0 degrees)

    while(RPS.Heading() < heading - 3 || RPS.Heading() > heading + 3)
    {
        if(RPS.Heading() > heading)
        {
            //pulse the motors for a short duration in the correct direction

            turn_right(motor_left_percent, motor_right_percent, right_turn_counts/45);
        }
        else if(RPS.Heading() < heading)
        {
            //pulse the motors for a short duration in the correct direction

            turn_left(motor_left_percent, motor_right_percent,left_turn_counts/45);

        }
        Sleep(200);
    }
    Sleep(200);


}

int main() {

    LCD.Clear(FEHLCD::Black);
    LCD.SetFontColor(FEHLCD::White);

    arm.SetMin(arm_Min);
    arm.SetMax(arm_Max);
    arm.SetDegree(default_degree);

    //Initialize RPS and declare any variables you might need
    float touch_x, touch_y;

    //Call this function to initialize the RPS to a course
    RPS.InitializeTouchMenu();

    //Wait for touchscreen to be pressed
    LCD.WriteLine("Press Screen to Start");
    while(!LCD.Touch(&touch_x, &touch_y));

    LCD.Clear();

    // Start a timer and enter a loop. Call the light_start() method.
    // When that method returns true or 20 seconds have passed, end the loop.
    int start_time = TimeNow();
    while(!light_start() && TimeNow() - start_time < 20.0);


    // Drive out of the starting area and go forward 10 inches.
    drive(motor_left_percent, motor_right_percent, 405 );

    // Turn left towards the ramp
    turn_left(motor_left_percent, motor_right_percent, left_turn_counts);


    // Drive towards the ramp, going forward 12 inches.
    drive(motor_left_percent, motor_right_percent, 486);

    // Turn left to face the ramp
    turn_left(motor_left_percent, motor_right_percent, left_turn_counts);
    check_heading(90);

    // Raise arm up so it does not hit the ramp
    arm.SetDegree(130);
    Sleep(sleep_time);

    // Drive up the ramp, going forward 28 inches.
    drive(motor_left_percent + 5, motor_right_percent + 5, 1134);
    check_heading(90);

    // Turn left to face the lever
    turn_left(motor_left_percent, motor_right_percent, left_turn_counts);
    check_heading(180);

    // Drive forward 2 inches
    drive(motor_left_percent, motor_right_percent, 81);

    // Lower arm
    arm.SetDegree(115);
    Sleep(sleep_time);

    // Raise arm
    arm.SetDegree(130);
    Sleep(sleep_time);

    // Back up 4 inches
    drive(-motor_left_percent, -motor_right_percent,162);

    // Turn left
    turn_left(motor_left_percent, motor_right_percent, left_turn_counts);
    check_heading(270);

    // Drive forward 28 inches
    drive(motor_left_percent, motor_right_percent, 1134);
    check_heading(270);
    check_y_minus(15.5);

    // Turn right
    turn_right(motor_left_percent, motor_right_percent, right_turn_counts);
    check_heading(180);

    // Lower arm
    arm.SetDegree(default_degree);
    Sleep(sleep_time);

    // Drive forward 4 inches
    drive(motor_left_percent, motor_right_percent, 162);
    check_x_minus(12.5);

    int num = light_red_or_blue();



    // Set arm to default_degree
    arm.SetDegree(default_degree);


    return 0;
}
