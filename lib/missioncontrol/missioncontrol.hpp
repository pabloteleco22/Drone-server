#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/action/action.h>

#include "../flag/flag.hpp"
#include <memory>
#include <functional>
#include <mutex>

enum class MissionControllerStatus : int{
    SUCCESS = 0,
    FAILURE = 1
};

struct MissionController {
    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    virtual MissionControllerStatus mission_control() = 0;
};

struct SearchController : public MissionController {
    SearchController(mavsdk::Telemetry *telemetry, mavsdk::Action *action,
                    const Flag *flag, std::function<void()> callback,
                    double position_rate, double separation);

    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    MissionControllerStatus mission_control() override;

    private:
        mavsdk::Telemetry *telemetry;
        mavsdk::Action *action;
        const Flag *flag;
        std::function<void()> callback;
        double position_rate;
        double separation;
        static std::mutex mut;
        static bool flag_found;
};