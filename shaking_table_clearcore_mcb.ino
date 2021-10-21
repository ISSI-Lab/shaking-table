/*
 * Title: StepAndDirection
 *
 * Objective:
 *    This example demonstrates control of a third party Step and
 *    Direction motor using a ClearCore motor connector.
 *    This example is NOT intended to be used with ClearPath servos.
 *    There are other examples created specifically for ClearPath.
 *
 * Description:
 *    This example enables a motor then commands a series of repeating
 *    moves to the motor.
 *
 * Requirements:
 * 1. A motor capable of step and direction must be connected to Connector M-0.
 * 2. The motor may optionally be connected to the MotorDriver's HLFB line if 
 *    the motor has a "servo on" type feedback feature.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

 /*
 * Title: ReadWrite
 *
 * Objective:
 *    This example demonstrates how to read from and write to a file on the
 *    SD card.
 *
 * Description:
 *    This example writes to a text file on the SD card then reads the file
 *    back to the USB serial port.
 *
 * Requirements:
 * ** A micro SD card installed in the ClearCore.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 *
 * Last Modified: 11/25/2020
 * This is a slightly modified version of the builtin Arduino SD ReadWrite sketch.
 * This example code is in the public domain. 
 */

#include "ClearCore.h"
#include <CSV_Parser.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <SD.h>

// Specifies which motor to move.
// Options are: ConnectorM0, ConnectorM1, ConnectorM2, or ConnectorM3.
#define motor ConnectorM0

// Select the baud rate to match the target serial device
#define baudRate 9600

File myFile;

// Define the velocity and acceleration limits to be used for each move
int velocityLimit = 10000; // pulses per sec
int accelerationLimit = 100000; // pulses per sec^2

// Declares our user-defined helper function, which is used to command moves to
// the motor. The definition/implementation of this function is at the  bottom
// of the example.
void MoveDistance(int distance);

const char input[] = "[true, 42, \"apple\"]";

void setup() {
    // Put your setup code here, it will only run once:

   // ---- SD read write code ----
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        // Wait for the USB serial port to open.
        continue; 
    }

    Serial.print("Initializing SD card...");

    if (!SD.begin()) {
        Serial.println("initialization failed!");
        while (true) {
            // We can't continue without a working SD card
            continue;
        }
    }
    Serial.println("initialization done.");

    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open("input.txt", FILE_WRITE);

    // If the file opened okay, write to it:
    if (myFile) {
        Serial.print("Writing to input.txt...");
        myFile.println("testing 1, 2, 3.");
        // Close the file:
        myFile.close();
        Serial.println("done.");
    } 
    else {
        // If the file didn't open, print an error:
        Serial.println("error opening input.txt");
        while (true) {
            continue;
        }
    }

    // Re-open the file for reading:
    myFile = SD.open("input.txt");
    if (myFile) {
        Serial.println("input.txt:");

        // Read from the file until there's nothing else in it:
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // Close the file:
        myFile.close();
    } 
    else {
        // If the file didn't open, print an error:
        Serial.println("error opening test.txt");
    }




   // ---- End of read/write code -----


   // --- read config json file ----

   demoParse();
   // ---end of json reading
 
    // Sets the input clocking rate.
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_NORMAL);

    // Sets all motor connectors into step and direction mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_STEP_AND_DIR);

    // These lines may be uncommented to invert the output signals of the 
    // Enable, Direction, and HLFB lines. Some motors may have input polarities 
    // that are inverted from the ClearCore's polarity.
    //motor.PolarityInvertSDEnable(true);
    //motor.PolarityInvertSDDirection(true);
    //motor.PolarityInvertSDHlfb(true);

    // Sets the maximum velocity for each move
    motor.VelMax(velocityLimit);

    // Set the maximum acceleration for each move
    motor.AccelMax(accelerationLimit);

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor.
    motor.EnableRequest(true);

    // Waits for HLFB to assert. Uncomment these lines if your motor has a 
    // "servo on" feature and it is wired to the HLFB line on the connector.
    //Serial.println("Waiting for HLFB...");
    //while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
    //    continue;
    //}
    Serial.println("Motor Ready");
}

void loop() {
    // Put your main code here, it will run repeatedly:

    // Move 6400 counts (positive direction), then wait 2000ms
    MoveDistance(6400);
    delay(2000);
    // Move 19200 counts farther positive, then wait 2000ms
    MoveDistance(19200);
    delay(2000);
    // Move back 12800 counts (negative direction), then wait 2000ms
    MoveDistance(-12800);
    delay(2000);
    // Move back 6400 counts (negative direction), then wait 2000ms
    MoveDistance(-6400);
    delay(2000);
    // Move back to the start (negative 6400 pulses), then wait 2000ms
    MoveDistance(-6400);
    delay(2000);
}

/*------------------------------------------------------------------------------
 * MoveDistance
 *
 *    Command "distance" number of step pulses away from the current position
 *    Prints the move status to the USB serial port
 *    Returns when step pulses have completed
 *
 * Parameters:
 *    int distance  - The distance, in step pulses, to move
 *
 * Returns: None
 */
void MoveDistance(int distance) {
    Serial.print("Moving distance: ");
    Serial.println(distance);

    // Command the move of incremental distance
    motor.Move(distance);

    // Waits for all step pulses to output
    Serial.println("Moving... Waiting for the step output to finish...");
    while (!motor.StepsComplete()) {
        continue;
    }

    Serial.println("Steps Complete");
}
//------------------------------------------------------------------------------


void demoParse() {
  Serial.println("parse");
  Serial.println("=====");

  JSONVar myArray = JSON.parse(input);

  // JSON.typeof(jsonVar) can be used to get the type of the variable
  if (JSON.typeof(myArray) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }

  Serial.print("JSON.typeof(myArray) = ");
  Serial.println(JSON.typeof(myArray)); // prints: array

  // myArray.length() can be used to get the length of the array
  Serial.print("myArray.length() = ");
  Serial.println(myArray.length());
  Serial.println();

  Serial.print("JSON.typeof(myArray[0]) = ");
  Serial.println(JSON.typeof(myArray[0]));

  Serial.print("myArray[0] = ");
  Serial.println(myArray[0]);
  Serial.println();

  Serial.print("myArray[1] = ");
  Serial.println((int) myArray[1]);
  Serial.println();

  Serial.print("myArray[2] = ");
  Serial.println((const char*) myArray[2]);
  Serial.println();

  Serial.println();
}
