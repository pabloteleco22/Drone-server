#pragma once

#include "../errorcontrol/error_control.hpp"
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

struct MissionController {
    /**
     * @brief Asynchronous method that controls the execution of the mission
    */
    virtual MissionControllerStatus mission_control() = 0;
};