add_library(MissionHelper missionhelper.cpp)
find_package(MAVSDK REQUIRED)
target_link_libraries(MissionHelper
    MAVSDK::mavsdk
)