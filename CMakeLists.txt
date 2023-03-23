cmake_minimum_required(VERSION 3.15)

# Specify your project's name
project(drone_server)
project(flag_test)

# Specify at least C++11, better C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable strict handling of warnings
add_definitions("-Wall -Wextra -Werror")
#add_definitions("-Wall -Wextra")

# Finds MAVSDK when installed system wide.
find_package(MAVSDK REQUIRED)

# Specify your app's executable name, and list of source files used to create it.
add_executable(drone_server
    main.cpp
)

add_executable(flag_test
    test/flag_test.cpp
)

add_subdirectory(flag)

# Specify your app's executable name and a list of linked libraries
target_link_libraries(drone_server
    MAVSDK::mavsdk             #All apps link against mavsdk library
    Flag
)

target_link_libraries(flag_test
    MAVSDK::mavsdk             #All apps link against mavsdk library
    Flag
)
