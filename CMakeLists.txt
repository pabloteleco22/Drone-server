cmake_minimum_required(VERSION 3.15)

# Specify your project's name
project(drone_server)

# Specify at least C++11, better C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable strict handling of warnings
#add_definitions("-Wall -Wextra -Werror")
add_definitions("-Wall -Wextra")

add_subdirectory(src/log)
add_subdirectory(src/missionhelper)
add_subdirectory(src/missioncontrol)
add_subdirectory(src/errorcontrol)
add_subdirectory(src/operation)
add_subdirectory(flag_search/src/flag)
add_subdirectory(flag_search/src/poly)
add_subdirectory(flag_search/src/missionhelper)
add_subdirectory(flag_search/src/missioncontrol)

# Finds MAVSDK when installed system wide.
find_package(MAVSDK REQUIRED)

#------------------- MAIN -------------------#
# Specify your app's executable name, and list of source files used to create it.
add_executable(drone_server
    flag_search/main.cpp
)

# Specify your app's executable name and a list of linked libraries
target_link_libraries(drone_server
    MAVSDK::mavsdk             #All apps link against mavsdk library
    Flag
    Log
    Poly
    MissionHelperFlagSearch
    MissionControlFlagSearch
    Operation
    ErrorControl
)

#------------------- TEST -------------------#
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

##------------------ Flag test ------------------##
add_executable(flag_test
    flag_search/test/flag_test.cpp
)

target_link_libraries(flag_test
    GTest::gtest_main
    GTest::gmock
    Flag
)

##------------------ Mission helper test ------------------##
add_executable(missionhelper_test
    flag_search/test/missionhelper_test.cpp
)

target_link_libraries(missionhelper_test
    GTest::gtest_main
    MissionHelperFlagSearch
)

##------------------ Operation test ------------------##
add_executable(operation_test
    test/operation_test.cpp
)

target_link_libraries(operation_test
    GTest::gtest_main
    Operation
)

##------------------ Log test ------------------##
add_executable(log_test
    test/log_test.cpp
)

target_link_libraries(log_test
    GTest::gtest_main
    Log
)

##------------------ Log demo ------------------##
add_executable(log_demo
    test/log_demo.cpp
)

target_link_libraries(log_demo
    Log
)

##------------------ Poly test ------------------##
add_executable(poly_test
    flag_search/test/poly_test.cpp
)

target_link_libraries(poly_test
    GTest::gtest_main
    Poly
)

include(GoogleTest)
gtest_discover_tests(flag_test)
gtest_discover_tests(log_test)
gtest_discover_tests(missionhelper_test)
gtest_discover_tests(poly_test)
