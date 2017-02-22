#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHRPS.h>
#include <FEHServo.h>
//sup

// Motors
FEHMotor left_motor(FEHMotor::Motor1, 12.0);
FEHMotor right_motor(FEHMotor::Motor3, 12.0);
//FEHServo arm(FEHServo::);

//Encoders
/*
DigitalEncoder right_encoder(FEHIO::);
DigitalEncoder left_encoder(FEHIO::);
*/

// Bump Switches
/*
DigitalInputPin front_left(FEHIO::);
DigitalInputPin front_right(FEHIO::);
DigitalInputPin back_left(FEHIO::);
DigitalInputPin back_right(FEHIO::);
/*

// CdS Cell
//AnalogInputPin cds_cell(FEHIO::);

// Optosensors
/*
AnalogInputPin left_sensor(FEHIO::);
AnalogInputPin middle_sensor(FEHIO::);
AnalogInputPin right_sensor(FEHIO::);

const double left_threshold = ;
const double middle_threshold = ;
const double right_threshold = ;
*/

bool touch_start() // Wait for screen touch
{
	float x, y;
	bool temp = false;
	while(!temp) {
		if(LCD.Touch(&x, &y)) {
			temp = true;
		}
	}
}



void drive(percent, counts)
{	
	// Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}



void turn_left(int percent, int counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent

    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors

    while((left_encoder.Counts() + right_encoder.Counts()) / 2 < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}



void turn_right(int percent, int counts) // using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent

    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder is less than counts,
    // keep running motors

    while((left_encoder.Counts() + right_encoder.Counts()) / 2 < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}



int main() {
	LCD.Clear(FEHLCD::Black);
	LCD.SetFontColor(FEHLCD::White);
	
	return 0;
}
