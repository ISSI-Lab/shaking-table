# NOTE:

Current status - this software can allow csv as the velocity and distance input and use a config file to determine which mode for control.
Now, acceleration control is not implemented yet. This relies on the actual weight of the equipment.

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
