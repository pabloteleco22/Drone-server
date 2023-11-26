# Drone-server
This repository contains a C++ framework for drone fleet management using the MAVLink protocol.
It has been tested with PX4-Autopilot, but it should work for any other MAVLINK compatible drone firmware.
There is also an application to search for a target that, given a defined area, divides it by the number of
available drones to minimize discovery time.
The framework is located in the src directory, and the application in flag_search.

The application uses a modified version of the solution developed by [Grabarchuk Viktor](https://github.com/dhmhd/poly-split/tree/master) to divide the search area.

## Dependencies
In order to compile this project it is necessary to have installed the [MAVSDK](https://github.com/mavlink/MAVSDK/releases) library, the CMake tools and a C++ 20 compiler.

## Cloning this repository
`git clone https://github.com/pabloteleco22/Drone-server.git`

## Compilation
`cmake -Bbuild .`

`cmake --build build -j4`

## Tests
There are more than 100 tests divided into several files within the [test](test) and [flag_search/test](flag_search/test) directory.
To run them all you must execute the script run_test.bash which, at the end, will show a summary with the results.
`./run_tests.bash`

## Logging
To save the logging messages when executing the search application you will need to create a logs directory where you call the runnable.

## Using Docker
You can build and execute this project on a docker container. To do this you have to install *docker* and *docker-compose* and execute the following command from the project root `HOST_UID=$(id -u) HOST_GID=$(id -g) docker-compose -f docker/docker-compose.yml up -d`. Then it will create a docker image and then a docker container.

To use the project in docker you can run `make docker-config-build` to make the build directory, `make docker-build` to build the project and `make docker-run ports="14540 14541 ..."` to run the application.