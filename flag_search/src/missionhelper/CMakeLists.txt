add_library(MissionHelperFlagSearch missionhelper.cpp)
find_package(MAVSDK REQUIRED)
target_link_libraries(MissionHelperFlagSearch
    MissionHelper
    MAVSDK::mavsdk
    Poly
)