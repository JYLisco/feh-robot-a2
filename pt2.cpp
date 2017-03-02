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
const int default_degree = 90; // Default degree servo should be at.


// Encoders
DigitalEncoder right_encoder(FEHIO::P0_7);
DigitalEncoder left_encoder(FEHIO::P2_7);

const float encoder_inch = 40.5; // Number of encoder counts for a single inch.
const int encoder_pulse = (int)encoder_inch/8; //Expected encoder counts for the IGWAN Motors when pulsing the drive command for the RPS coordinates.

const int turn_right_counts = 235; // Expected encoder counts for the right IGWAN motor.
const int turn_left_counts = 228; // Expected encoder counts for the left IGWAN motor.
const int turn_right_pulse = turn_right_counts/45; //Expected encoder counts for the right IGWAN Motor when pulsing the turn command.
const int turn_left_pulse = turn_left_counts/45; //Expected encoder counts for the left IGWAN Motor when pulsing the turn command for the RPS.

// Bump Switches
/*
DigitalInputPin front_left(FEHIO::);
DigitalInputPin front_right(FEHIO::);
DigitalInputPin back_left(FEHIO::);
DigitalInputPin back_right(FEHIO::);
*/

//CdS Cell
AnalogInputPin cds_cell(FEHIO::P1_6); // CdS Cell

const double red_light = 0.3; // max value
const double blue_light = 0.7; // max value

// Optosensors
/*
AnalogInputPin left_sensor(FEHIO::);
AnalogInputPin middle_sensor(FEHIO::);
AnalogInputPin right_sensor(FEHIO::);

const double left_threshold = 2.0;
const double middle_threshold = 2.0;
const double right_threshold = 2.0;
*/

//RPS Constants
const float tolerance_XY = .15; //Tolerance when determining position using the RPS.
const int tolerance_heading = 3; //Tolerance when determining heading using the RPS

const int heading_right = 0; //Heading angle for going in the direction from the start to the satellite.
const int heading_up = 90; //Heading angle for going in the direction up the ramp.
const int heading_left = 180; //Heading angle for going in the direction from the satellite towards the start.
const int heading_down = 270; //Heading angle for going in the direction down the ramp.

const float x_bin_light = 12.2, y_bin_light = 16.1;
const float x_lever = 0, y_lever = 0;
const float x_button = 0, y_button = 0;



//Other constants
const int sleep_time = 200; // The amount of time to sleep between functions.
const int timer_start = 20; //The amount of time the robot will wait to move should it not receive CdS Cell input.



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
    if(cds_cell.Value() < red_light)
    {
        LCD.WriteLine("Light is red."); // return 1
        light = 1;
    }
    else if(cds_cell.Value() < blue_light)
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
/*
void follow_line(int motor_left_percent, int motor_right_percent)
{
    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);

    if(middle_sensor.Value() < middle_threshold)
    {
        if(left_sensor.Value() < left_threshold)
        {
            right_motor.Stop();
            if(left_sensor.Value() < left_threshold)
            {
                Sleep(sleep_time);
            }
        }
        else if(right_sensor.Value() < right_threshold)
        {
            left_motor.Stop();
            if(right_sensor.Value() < right_threshold)
            {
                Sleep(sleep_time);
            }
        }
    }
}*/

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
        Sleep(sleep_time);

    }
}

void check_x_plus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - tolerance_XY || RPS.X() > x_coordinate + tolerance_XY)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, encoder_pulse);
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, encoder_pulse);
        }
    }
}

void check_x_minus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - tolerance_XY || RPS.X() > x_coordinate + tolerance_XY)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, encoder_pulse);
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, encoder_pulse);
        }
    }
}

void check_y_minus(float y_coordinate) //using RPS while robot is in the -y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - tolerance_XY || RPS.Y() > y_coordinate + tolerance_XY)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, encoder_pulse);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, encoder_pulse);
        }
    }
}

void check_y_plus(float y_coordinate) //using RPS while robot is in the +y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - tolerance_XY || RPS.Y() > y_coordinate + tolerance_XY)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(-motor_left_percent, -motor_right_percent, encoder_pulse);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

            drive(motor_left_percent, motor_right_percent, encoder_pulse);
        }
    }
}


void check_heading(float heading) //using RPS
{
    //you will need to fill out this one yourself and take into account
    //the edge conditions (when you want the robot to go to 0 degrees
    //or close to 0 degrees)

    while(RPS.Heading() < heading - tolerance_heading || RPS.Heading() > heading + tolerance_heading)
    {
        if(RPS.Heading() > heading)
        {
            //pulse the motors for a short duration in the correct direction

            turn_right(motor_left_percent, motor_right_percent, turn_right_pulse);
        }
        else if(RPS.Heading() < heading)
        {
            //pulse the motors for a short duration in the correct direction

            turn_left(motor_left_percent, motor_right_percent,turn_left_pulse);

        }
        Sleep(sleep_time);
    }
    Sleep(sleep_time);


}

void arm_adjust(int angle)
{
    arm.SetDegree(angle);
    Sleep(sleep_time);
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
    while(!light_start() && TimeNow() - start_time < timer_start);


    // Drive out of the starting area and go forward 10 inches.
    drive(motor_left_percent, motor_right_percent, (int)encoder_inch*10 );

    // Turn left towards the ramp
    turn_left(motor_left_percent, motor_right_percent, turn_left_counts);


    // Drive towards the ramp, going forward 12 inches.
    drive(motor_left_percent, motor_right_percent, (int)encoder_inch*12);

    // Turn left to face the ramp
    turn_left(motor_left_percent, motor_right_percent, turn_left_counts);
    check_heading(heading_up);

    // Raise arm up so it does not hit the ramp
    arm_adjust(130);

    // Drive up the ramp, going forward 28 inches.
    drive(motor_left_percent + 5, motor_right_percent + 5, (int)encoder_inch*28);
    check_heading(heading_up);

    // Turn left to face the lever
    turn_left(motor_left_percent, motor_right_percent, turn_left_counts);
    check_heading(heading_left);

    // Drive forward 2 inches towards the lever.
    drive(motor_left_percent, motor_right_percent, (int)encoder_inch*2);

    // Lower arm to hit the lever.
    arm_adjust(115);
    Sleep(sleep_time);

    // Raise arm back to upper position.
    arm_adjust(130);

    // Lower arm to hit the lever.
    arm_adjust(115);
    Sleep(sleep_time);

    // Raise arm back to upper position.
    arm_adjust(130);

    // Back up 4 inches towards the ramp entrance.
    drive(-motor_left_percent, -motor_right_percent,(int)encoder_inch*4);

    // Turn left towards the ramp.
    turn_left(motor_left_percent, motor_right_percent, turn_left_counts);
    check_heading(heading_down);

    // Drive forward 28 inches down the ramp.
    drive(motor_left_percent, motor_right_percent, (int)encoder_inch*28);
    check_heading(heading_down);
    check_y_minus(y_bin_light);

    // Turn right towards the bin light.
    turn_right(motor_left_percent, motor_right_percent, turn_right_counts);
    check_heading(heading_left);

    // Lower arm back to default position.
    arm_adjust(default_degree);

    // Drive forward 4 inches towards the bin light.
    drive(motor_left_percent, motor_right_percent, (int)encoder_inch*4);
    check_x_minus(x_bin_light);

    int num = light_red_or_blue();



    // Set arm to default_degree
    arm.SetDegree(default_degree);


    return 0;
}
