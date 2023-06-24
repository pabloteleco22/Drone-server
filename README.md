# Drone-server
This repository contains a C++ framework for drone fleet management using the MAVLink protocol.
It has been tested with PX4-Autopilot, but it should work for any other MAVLINK compatible drone firmware.
There is also an application to search for a target that, given a defined area, divides it by the number of
available drones to minimize discovery time.
The framework is located in the src directory, and the application in flag_search.

The application uses a modified version of the repository available at https://github.com/dhmhd/poly-split/tree/master
to divide the search area.

## Dependencies
In order to compile this project it is necessary to have installed the MAVSDK library,
available at https://github.com/mavlink/MAVSDK/releases, the CMake tools and a C++ 20 compiler.

## Cloning this repository
`git clone https://github.com/pabloteleco22/Drone-server.git`

## Compilation
`cmake -Bbuild .`

`cmake --build build -j4`

## Tests
There are more than 100 tests divided into several files within the
[test](test) and
[flag_search/test](flag_search/test) directory.
To run them all you must execute the script run_test.bash which, at the end, will show a summary with the results.
`./run_tests.bash`

## Logging
To save the logging messages when executing the search application you will need to create a logs directory where
you call the runnable.
