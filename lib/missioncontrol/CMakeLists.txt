add_library(MissionControl missioncontrol.cpp)
find_package(MAVSDK REQUIRED)
target_link_libraries(MissionControl
    MAVSDK::mavsdk
)