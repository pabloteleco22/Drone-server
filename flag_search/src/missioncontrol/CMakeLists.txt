add_library(MissionControlFlagSearch missioncontrol.cpp)
find_package(MAVSDK REQUIRED)
target_link_libraries(MissionControlFlagSearch
    MissionControl
    MAVSDK::mavsdk
    ErrorControl
)