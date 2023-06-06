#pragma once

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/action/action.h>

#include "../flag/flag.hpp"
#include "../errorcontrol/error_control.hpp"
#include <memory>
#include <functional>
#include <mutex>
#include <string>

using std::string;

class MissionControllerStatus : public ProRetCod {
    using ProRetCod::ProRetCod;
};

struct MCSSuccess : public MissionControllerStatus {
    MCSSuccess() : MissionControllerStatus(code, message) {}

    static const int code;
    static const string message;
};

struct MCSSuscribePositionFailure : public MissionControllerStatus {
    MCSSuscribePositionFailure () : MissionControllerStatus(code, message) {}

    static const int code;
    static const string message;
};

struct MissionController {
    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    virtual MissionControllerStatus mission_control() = 0;
};

struct SearchController : public MissionController {
    SearchController(mavsdk::Telemetry *telemetry,
                    const Flag *flag,
                    std::function<void(Flag::Position, bool)> callback,
                    double position_rate, double detection_radius);

    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    MissionControllerStatus mission_control() override;

    private:
        mavsdk::Telemetry *telemetry;
        const Flag *flag;
        std::function<void(Flag::Position, bool)> callback;
        double position_rate;
        double detection_radius;
        static std::mutex mut;
        static bool flag_found;
};