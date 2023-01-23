# NOTE:

Current status - this software can allow csv as the velocity and distance input and use a config file to determine which mode for control.
Now, acceleration control is not implemented yet. This relies on the actual weight of the equipment.

# Hardware
User Interface (UI) control: Touch screen + Raspberry Pi
Micro Controller Board (MCB) for motor: Clearcore

# Functionalities
1. Upload an input file from a drive and then click start to execute the motor motions.
2. Direct input X and Y values to move motors
3. Click the button to stop the movement
4. Status check on MCB and show message on UI
 

# Shaking Table Controller by Clearcore 

Version: 00.00.01
This is the clearcore arduino wrapper's shaking table codes.

There are two input files: (1) a CSV file - for all the moving related value; (2) a JSON file for config the type of movement.

## Three types of movement:
**(1) Move based on the distances**
An example of input CSV file is as follows.

timestamp, x_distance, y_distance
0, 0, 0
1000, 5000, 2000

Currently, the units for above are milli-seconds, counts, counts.

**(2) Move based on velocity (Not implemented yet. Need to define many items)**

An example of input CSV file is as follows.

timestamp, x_velocity, y_velocity
0, 0, 0
1000, 5000, 2000

**(3) Move based on acceleration (Not implemented yet. Need to define many items)**

An example of input CSV file is as follows.

timestamp, x_acceleration, y_acceleration
0, 0, 0
1000, 5000, 2000
