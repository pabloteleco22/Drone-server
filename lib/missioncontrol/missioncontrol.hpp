#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include "../flag/flag.hpp"
#include <memory>
#include <functional>

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

struct SearchController : MissionController {
    SearchController(std::shared_ptr<mavsdk::System> system, const Flag *flag,
                    std::function<void()> callback, double position_rate,
                    double separation);
    ~SearchController();

    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    MissionControllerStatus mission_control() override;

    private:
        std::shared_ptr<mavsdk::System> system;
        const Flag *flag;
        std::function<void()> callback;
        double position_rate;
        mavsdk::Telemetry *telemetry;
        double separation;
};