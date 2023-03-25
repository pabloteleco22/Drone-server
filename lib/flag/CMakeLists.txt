add_library(Flag flag.cpp)

find_package(MAVSDK REQUIRED)

target_link_libraries(Flag
    MAVSDK::mavsdk             #All apps link against mavsdk library
)