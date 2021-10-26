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
 * ** ClearPath Manual (DC Power): https://www.teknic.com/files/downloads/clearpath_user_manual.pdf
 * ** ClearPath Manual (AC Power): https://www.teknic.com/files/downloads/ac_clearpath-mc-sd_manual.pdf
 *
 * Last Modified: 1/21/2020
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"
#include <CSV_Parser.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <SD.h>
#include <string.h>

// Specifies which motor to move.
// Options are: ConnectorM0, ConnectorM1, ConnectorM2, or ConnectorM3.
#define motorX ConnectorM0
#define motorY ConnectorM1

// Select the baud rate to match the target serial device
#define baudRate 9600

File inputCSVFile;
File logFile;

// Define the velocity and acceleration limits to be used for each move
int velocityLimit = 10000; // pulses per sec
int accelerationLimit = 100000; // pulses per sec^2

// Declares our user-defined helper function, which is used to command moves to
// the motor. The definition/implementation of this function is at the  bottom
// of the example.
void MoveXDistance(int distance);
void MoveYDistance(int distance);
void initSD();
void initLogFile();
void readCSVFile();
void readJSONFile();
void writeLog(String logStr);
void setConfig();
bool MoveAtVelocity(int32_t velocity);

char csvStr[10000]; // we need to check the limitation later
char jsonStr[1000];
JSONVar jsonObject;
int controlMode = 0; //0: distance and 1: velocity and 2: acceleration (default distance mode => 0)
int moveMode = 0; //0: one time; 1: cycling (repeat) (default is one-time => 0)

float *xVals; //This is used to save distance or velocity or acceleration at x-axis depends on mode
float *yVals; //This is used to save distance or velocity or acceleration at y-axis depends on mode
int32_t *longTimestamp;
int numOfRecords = 0;

//The headline string of the CSV file
char X_DISTANCE[] = "x_distance";
char Y_DISTANCE[] = "y_distance";
char X_VELOCITY[] = "x_velocity";
char Y_VELOCITY[] = "y_velocity";
char X_ACCELERATION[] = "x_acceleration";
char Y_ACCELERATION[] = "y_acceleration";
char TIMESTAMP[] = "timestamp";
char moveState = 0;

void setup() {
    // Put your setup code here, it will only run once:

   // ---- SD read write code ----
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        // Wait for the USB serial port to open.
        continue; 
    }

    initSD();
    initLogFile();
    readCSVFile();
    Serial.print(csvStr);
    delay(1000);

    readJSONFile();
    setConfig();
    
    CSV_Parser cp(csvStr, /*format*/ "Lff");
    numOfRecords = cp.getRowsCount();
    Serial.println("number of record");
    Serial.print(numOfRecords);
    
    if (controlMode == 0) {
        xVals = (float*)cp[X_DISTANCE]; //This is used to save distance or velocity or acceleration at x-axis depends on mode
        yVals = (float*)cp[Y_DISTANCE];  //This is used to save distance or velocity or acceleration at y-axis depends on mode
        longTimestamp = (int32_t*)cp[TIMESTAMP];   
    }


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
   
    //controlMode = 1;
    
    if (controlMode == 0) {
        // Sets the maximum velocity for each move
        motorX.VelMax(velocityLimit);
        motorY.VelMax(velocityLimit);
    }
    if (controlMode == 1) {
        // Set the motor's HLFB mode to bipolar PWM
        motorX.HlfbMode(MotorDriver::HLFB_MODE_HAS_BIPOLAR_PWM);
        // Set the HFLB carrier frequency to 482 Hz
        motorX.HlfbCarrier(MotorDriver::HLFB_CARRIER_482_HZ);
        // Set the motor's HLFB mode to bipolar PWM
        motorY.HlfbMode(MotorDriver::HLFB_MODE_HAS_BIPOLAR_PWM);
        // Set the HFLB carrier frequency to 482 Hz
        motorY.HlfbCarrier(MotorDriver::HLFB_CARRIER_482_HZ);
    }
    // Set the maximum acceleration for each move
    motorX.AccelMax(accelerationLimit);
    motorY.AccelMax(accelerationLimit);

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    Serial.begin(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = millis();
    while (!Serial && millis() - startTime < timeout) {
        continue;
    }

    // Enables the motor.
    motorX.EnableRequest(true);
    motorY.EnableRequest(true);

    // Waits for HLFB to assert. Uncomment these lines if your motor has a 
    // "servo on" feature and it is wired to the HLFB line on the connector.
    //Serial.println("Waiting for HLFB...");
    //while (motor.HlfbState() != MotorDriver::HLFB_ASSERTED) {
    //    continue;
    //}
    Serial.println("Motor X Ready");
    Serial.println("Motor Y Ready");
}

void loop() {
  
    // Put your main code here, it will run repeatedly:
    if (controlMode == 0) {
        if (moveState == 0) {
            writeLog("movement state == start");
            for (int i = 0; i < numOfRecords; i++) {
                 Serial.print(longTimestamp[i]);             
                 Serial.print(" - ");
                 Serial.print(xVals[i]);          
                 Serial.print(" - ");
                 Serial.print(yVals[i]);           
                 Serial.print(" - ");
                 MoveXDistance(xVals[i]);
                 MoveXDistance(yVals[i]);
                 if (i+1 < numOfRecords) {
                     delay(longTimestamp[i+1] - longTimestamp[i]);
                 }
            }
            moveState = 1;
        } else {
            //writeLog("movement state == finished");
        }

    } else if (controlMode == 1) {
        // Move at 1,000 steps/sec for 2000ms
        MoveYAtVelocity(1000);
        delay(2000);
        // Move at -5,000 steps/sec for 2000ms
        MoveYAtVelocity(-5000);
        delay(2000);
        // Move at 10,000 steps/sec for 2000ms
        MoveYAtVelocity(10000);
        delay(2000);
        // Move at -10,000 steps/sec for 2000ms
        MoveYAtVelocity(-10000);
        delay(2000);
        // Command a 0 steps/sec velocity to stop motion for 2000ms
        MoveYAtVelocity(0);
        delay(2000);
      
    } else {
      
    }

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
void MoveXDistance(int distance) {
    Serial.print("Moving distance: ");
    Serial.println(distance);

    // Command the move of incremental distance
    motorX.Move(distance);

    // Waits for all step pulses to output
    Serial.println("Moving... Waiting for the step output to finish...");
    while (!motorX.StepsComplete()) {
        continue;
    }

    Serial.println("Steps Complete");
}

void MoveYDistance(int distance) {
    Serial.print("Moving distance: ");
    Serial.println(distance);

    // Command the move of incremental distance
    motorY.Move(distance);

    // Waits for all step pulses to output
    Serial.println("Moving... Waiting for the step output to finish...");
    while (!motorY.StepsComplete()) {
        continue;
    }

    Serial.println("Steps Complete");
}
//------------------------------------------------------------------------------


/*------------------------------------------------------------------------------
 * MoveAtVelocity
 *
 *    Command the motor to move at the specified "velocity", in pulses/second.
 *    Prints the move status to the USB serial port
 *
 * Parameters:
 *    int velocity  - The velocity, in step pulses/sec, to command
 *
 * Returns: True/False depending on whether the move was successfully triggered.
 */
bool MoveXAtVelocity(int velocity) {
    // Check if an alert is currently preventing motion
    if (motorX.StatusReg().bit.AlertsPresent) {
        Serial.println("Motor status: 'In Alert'. Move Canceled.");
        return false;
    }

    Serial.print("Moving at velocity: ");
    Serial.println(velocity);

    // Command the velocity move
    motorX.MoveVelocity(velocity);

    // Waits for the step command to ramp up/down to the commanded velocity. 
    // This time will depend on your Acceleration Limit.
    Serial.println("Ramping to speed...");
    while (!motorX.StatusReg().bit.AtTargetVelocity) {
        continue;
    }

    Serial.println("At Speed");
    return true; 
}

bool MoveYAtVelocity(int velocity) {
    // Check if an alert is currently preventing motion
    if (motorY.StatusReg().bit.AlertsPresent) {
        Serial.println("Motor status: 'In Alert'. Move Canceled.");
        return false;
    }

    Serial.print("Moving at velocity: ");
    Serial.println(velocity);

    // Command the velocity move
    motorY.MoveVelocity(velocity);

    // Waits for the step command to ramp up/down to the commanded velocity. 
    // This time will depend on your Acceleration Limit.
    Serial.println("Ramping to speed...");
    while (!motorY.StatusReg().bit.AtTargetVelocity) {
        continue;
    }

    Serial.println("At Speed");
    return true; 
}
//------------------------------------------------------------------------------


void initSD() {
    Serial.print("Initializing SD card...");

    if (!SD.begin()) {
        Serial.println("initialization failed!");
        while (true) {
            // We can't continue without a working SD card
            continue;
        }
    }
    Serial.println("initialization SD done.");
}

void initLogFile() {
    Serial.println("initialization Log File.");
    logFile = SD.open("error.log", FILE_WRITE);

    // If the file opened okay, write to it:
    if (logFile) {
        Serial.println("Starting to write the log...");
        logFile.println("Starting to write the log...");
        // Close the file:
        logFile.close();
    } else {
        // If the file didn't open, print an error:
        Serial.println("ERROR: opening info.log");
        while (true) {
            continue;
        }
    }
    Serial.println("Done with initialization Log File.");
}

void readCSVFile() {
    // Re-open the file for reading:
    inputCSVFile = SD.open("input.csv");
    if (inputCSVFile) {
        //Serial.println();
        writeLog("Startiing to read the input.csv file");

          // Rewind file so test data is not appended.
        //inputCSVFile.seek(0);
        char ltr;
        int count = 0;

        // Read from the file until there's nothing else in it:
        while (inputCSVFile.available()) {
            //Serial.write(inputCSVFile.read());
            ltr = inputCSVFile.read();
            Serial.print(ltr);
            csvStr[count] =  ltr;
            count++;
        }
        writeLog(csvStr);
        // Close the file:
        inputCSVFile.close();
    } 
    else {
        // If the file didn't open, print an error:
        Serial.println("ERROR: cannot opening input.csv");
    }
}

void writeLog(String logStr) {
    logFile = SD.open("info.log", FILE_WRITE);

    // If the file opened okay, write to it:
    if (logFile) {
        Serial.println(logStr);
        logFile.println(logStr);
        // Close the file:
        logFile.close();
    } else {
        Serial.println("ERROR: writing a log to info.log");
    }
}

void readJSONFile() {
    // Re-open the file for reading:
    Serial.println("reading config.sjon file...");
    File configFile = SD.open("config1.txt");
    if (configFile) {
        //Serial.println();
        writeLog("Startiing to read the config.json file");

        char ch;
        int cnt = 0;

        // Read from the file until there's nothing else in it:
        while (configFile.available()) {
            ch = configFile.read();
            Serial.print(ch);
            jsonStr[cnt] =  ch;
            cnt++;
        }
        writeLog(jsonStr);
        // Close the file:
        configFile.close();
    } 
    else {
        // If the file didn't open, print an error:
        Serial.println("error opening config.json");
    }
}

void setConfig() {
  writeLog("starting to parse JSON file");
  
  jsonObject = JSON.parse(jsonStr);

  // JSON.typeof(jsonVar) can be used to get the type of the variable
  if (JSON.typeof(jsonObject) == "undefined") {
      writeLog("infrginrd - starting to parse JSON file");
      return;
  }

  if (jsonObject.hasOwnProperty("control_mode")) {
      writeLog("INFO: Has control mode");
      writeLog((const char*)jsonObject["control_mode"]);
      //writeLog(("Control Mode is " + (const char*)jsonObject["control_mode"] );

      if (strcmp((const char*)jsonObject["control_mode"], "distance") == 0) {
          controlMode = 0;
      } else if (strcmp((const char*)jsonObject["control_mode"], "velocity") == 0) {
          controlMode = 1;
      } else {
          controlMode = 2;
      }
  }

  if (jsonObject.hasOwnProperty("move_mode")) {
      writeLog("INFO: Has moving mode");
      writeLog((const char*)jsonObject["move_mode"]);
      //writeLog(("Control Mode is " + (const char*)jsonObject["move_mode"] );

      if (strcmp((const char*)jsonObject["move_mode"], "one") == 0) {
          moveMode = 0;
      } else if (strcmp((const char*)jsonObject["move_mode"], "cycle") == 0) {
          moveMode = 1;
      } 
  }

}
