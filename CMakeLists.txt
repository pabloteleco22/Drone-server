cmake_minimum_required(VERSION 3.15)

# Specify your project's name
project(drone_server)

# Specify at least C++11, better C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable strict handling of warnings
add_definitions("-Wall -Wextra -Werror")
#add_definitions("-Wall -Wextra")

add_subdirectory(flag)

# Finds MAVSDK when installed system wide.
find_package(MAVSDK REQUIRED)

# Specify your app's executable name, and list of source files used to create it.
add_executable(drone_server
    main.cpp
)

# Specify your app's executable name and a list of linked libraries
target_link_libraries(drone_server
    MAVSDK::mavsdk             #All apps link against mavsdk library
    Flag
)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(flag_test
    test/flag_test.cpp
)

target_link_libraries(flag_test
    GTest::gtest_main
    GTest::gmock
    MAVSDK::mavsdk             #All apps link against mavsdk library
    Flag
)

include(GoogleTest)
gtest_discover_tests(flag_test)
