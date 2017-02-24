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
const int sleep_time = 500;


bool touch_start() // Wait for screen touch
{
    float x, y;
    bool temp = false;
    while(!temp) {
        if(LCD.Touch(&x, &y)) {
            temp = true;
        }
    }
    return temp;
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

void servo_calibrate()
{
    // Around 90 degrees seems like a good starting point
    
    arm.SetDegree(0);
    Sleep(2000);
    arm.SetDegree(180);
    Sleep(2000);

    int degree = 90;
    arm.SetDegree(90);

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
            //Sleep(8000);
        
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
