/**
* For use with the Arduino/Genuino IDE
* 
* Program for controlling BYJ48 Motors (and only these!) with step angle of 5,625°~
* For other motors (1.8° step angle etc.) the values of steps_per_revolutions and motor1AM (motor2AM, motor3AM) have to be changed accordingly
*
* License information:
* The MIT License (MIT)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the Software without restriction, including without limitation
* he rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
* to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of 
* the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
**/

#include <Stepper.h>

enum EMotor {
	MOTOR_ALL,
	MOTOR_ONE,
	MOTOR_TWO,
	MOTOR_THREE
};

// We set the steps per revolution to 64 because the step angle of the motor is 5,625 per revolution, so a full turn is defined as 360°/5.625 = 64 (???)
const int steps_per_revolution = 64;

// Motor1 pins 1 to 4
const int m1_p1 = 10;
const int m1_p2 = 11;
const int m1_p3 = 12;
const int m1_p4 = 13;

// Motor2 pins 5 to 8
const int m2_p1 = 5;
const int m2_p2 = 6;
const int m2_p3 = 7;
const int m2_p4 = 8;

// Motor3 pins 9 to 12
const int m3_p1 = 9;
const int m3_p2 = 10;
const int m3_p3 = 11;
const int m3_p4 = 12;

// Button input, pin 13
const int buttonPin = 2;

// Button pressed(1) or released(0)?
// int savedButtonState = 0;
int aperatureActive = 0;
unsigned int timeDuration = 0;

// This the approx.(!) time in milliseconds after the aperature closes automatically
// TODO: Is there a time library or can we use C time?
const int automaticCollapseMS = 10000;

// These constants define the amount the motorknob is rotated, it needs to be a multiple of 32, 2048 / 32 = 64 (one turn)
// So for a 1.5x turn we need to set it to 2048 + 1024 = 3072
const int motor1AM = 2048;
const int motor2AM = 2048;
const int motor3AM = 2048;

Stepper motor1(steps_per_revolution, m1_p1, m1_p2, m1_p3, m1_p4);
//Stepper motor2(steps_per_revolution, m2_p1, m2_p2, m2_p3, m2_p4);
//Stepper motor3(steps_per_revolution, m3_p1, m3_p2, m3_p3, m3_p4);

void SetMotorSpeed(EMotor motor, long speed) {
	switch (motor) {
		case MOTOR_ONE:
			motor1.setSpeed(speed);
		break;
		//case MOTOR_TWO:
		//	motor2.setSpeed(speed);
		//break;
		//case MOTOR_THREE:
		//	motor3.setSpeed(speed);
		//break;
		case MOTOR_ALL:
			motor1.setSpeed(speed);
			//motor2.setSpeed(speed);
			//motor3.setSpeed(speed);
		break;
		default:
		break;
	}
}

// @revolutions = positive amounts move the motorknob forwards, negative amounts backwards
// remember that .step() is blocking, so if we want to move all motors at once we need a different approach/function
// with revolution deltas
void MoveMotor(EMotor motor, int revolutions) {
	switch (motor) {
		case MOTOR_ONE:
			motor1.step(revolutions);
		break;
		//case MOTOR_TWO:
		//	motor2.step(revolutions);
		//break;
		//case MOTOR_THREE:
		//	motor3.step(revolutions);
		//break;
		case MOTOR_ALL:
			motor1.step(revolutions);
			//motor2.step(revolutions);
			//motor3.step(revolutions);
		break;
		default:
		break;
	}
}

void setup() {
	// Setup the button pin as input
	pinMode(buttonPin, INPUT);
	
	// Set the motor speed for all motor, change if necessary calling each one seperately
	SetMotorSpeed(MOTOR_ALL, 500);		// Check C++98 enum?
	
	Serial.begin(9600);
	
	Serial.print("Program starting");
}

void loop() {
	int currentButtonState = digitalRead(buttonPin);
	
	if (aperatureActive == 0 && currentButtonState == HIGH) {
		Serial.print("Button pressed, starting motors forward");
		// Someone pressed the button while the aperature was inactive, start rolling out..
		
		// Step the motors
		// Remember: For now this moves the motors one after another.. see MoveMotor comments
		MoveMotor(MOTOR_ONE, motor1AM);
		//MoveMotor(MOTOR_TWO, motor2AM);
		//MoveMotor(MOTOR_THREE, motor3AM);
		
		// Save the state
		aperatureActive = 1;
	}
	else if (aperatureActive == 1 && currentButtonState == HIGH) {
		Serial.print("Button pressed, starting motors backwards");
		// The aperature was active  and the button was pressed, roll in
		
		// Step the motors backwards, so we start with motor 3
		//MoveMotor(MOTOR_THREE, -1 * motor3AM);
		//MoveMotor(MOTOR_TWO, -1 * motor2AM);
		MoveMotor(MOTOR_ONE, -1 * motor1AM);
		
		// Save the state
		aperatureActive = 0;
		
		// Set time to null
		timeDuration = 0;
	}
	else if (aperatureActive == 1 && timeDuration >= automaticCollapseMS) {
		Serial.print("Time run out, starting motors backwards");
		// Automatic collapse after time duration, roll in
		
		// Step the motors backwards, so we start with motor 3
		//MoveMotor(MOTOR_THREE, -1 * motor3AM);
		//MoveMotor(MOTOR_TWO, -1 * motor2AM);
		MoveMotor(MOTOR_ONE, -1 * motor1AM);
		
		// Save the state
		aperatureActive = 0;
		
		// Set time to null
		timeDuration = 0;
	}
	else if (aperatureActive == 1) {
		delay(1);	// Delay for a milliseconds
		timeDuration = timeDuration + 1;	// And save that
	}
	else {
		// Aperature is not doing anything, Sleep to preserve power?
		delay(1);
	}
}

