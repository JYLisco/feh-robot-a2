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

const int sleep_time = 200; // sleep between drive functions

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

bool light_start() // detects if light is on or off
{
    bool start = false;
    if (cds_cell.Value() < red_light)
    {
        start = true;
    }
    return start;
}

int red_or_blue_light() // detects if light is red or blue
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



void drive(int left_percent, int right_percent, int counts)
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(right_percent);
    left_motor.SetPercent(left_percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(sleep_time);
}



void turn_left(int left_percent, int right_percent, int counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent

    right_motor.SetPercent(right_percent);
    left_motor.SetPercent(-left_percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors

    while((left_encoder.Counts() + right_encoder.Counts()) / 2 < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(sleep_time);
}



void turn_right(int left_percent, int right_percent, int counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent

    right_motor.SetPercent(-right_percent);
    left_motor.SetPercent(left_percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors

    while((left_encoder.Counts() + right_encoder.Counts()) / 2 < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(sleep_time);
}



int main() {
    LCD.Clear(FEHLCD::Black);
    LCD.SetFontColor(FEHLCD::White);

    arm.SetMin(500);
    arm.SetMax(2480);

    int right_turn_counts = 260;
    int left_turn_counts = 275;
    int left_percent = 25;
    int right_percent = 25;
    int percent = 25;

    // wait for light to turn on or 20 seconds to pass
    int start_time = TimeNow();
    while(!light_start() && TimeNow() - start_time < 20.0);



    drive(left_percent, right_percent, 405); // 10 inches forward
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    turn_left(left_percent, right_percent, left_turn_counts); // turn left
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    drive(left_percent, right_percent, 486); // 12 inches
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    turn_left(left_percent, right_percent, left_turn_counts); // turn left
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    drive(left_percent, right_percent, 1215); // 30 inches forward
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    turn_right(left_percent, right_percent, right_turn_counts); // turn right
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    drive(left_percent, right_percent, 324); // 8 inches forward
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    turn_right(left_percent, right_percent, right_turn_counts); // turn right
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    drive(-left_percent, -right_percent, 810); // 10 inches backward
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    drive(left_percent, right_percent, 486); // 12 inches forward
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    turn_right(left_percent, right_percent, right_turn_counts); // turn right
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    drive(left_percent, right_percent, 608); // 15 inches
    LCD.Write("LE: ");
    LCD.WriteLine(left_encoder.Counts());
    LCD.Write("RE: ");
    LCD.WriteLine(right_encoder.Counts());
    LCD.WriteLine(" ");

    return 0;
}
