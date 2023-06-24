#include "missioncontrol.hpp"

#include <cmath>

/** MissionControllerStatus **/
const int MCSSuscribePositionFailure::code{1};
const string MCSSuscribePositionFailure::message{"unable to suscribe position"};

/** Search Controller **/
bool SearchController::flag_found{false};
std::mutex SearchController::mut{};

SearchController::SearchController(mavsdk::Telemetry *telemetry,
                    const Flag *flag,
                    std::function<void(Flag::Position, bool)> callback,
                    double position_rate, double detection_radius) {
    this->telemetry = telemetry;
    this->flag = flag;
    this->callback = callback;
    this->position_rate = position_rate;
    this->detection_radius = detection_radius;
}

MissionControllerStatus SearchController::mission_control() {
    // Establecer a un segundo la frecuencia de transmisión de la posición
    mavsdk::Telemetry::Result result{telemetry->set_rate_position(position_rate)};
    if (result != mavsdk::Telemetry::Result::Success) {
        MCSSuscribePositionFailure failure;
        return failure;
    }

    // Pedir regularmente la posición de los drones
    telemetry->subscribe_position(
        [this](mavsdk::Telemetry::Position pos) {
        Flag::Position flag_pos{this->flag->get_flag_position()};
        bool flag_found;
        bool flag_found_by_me{false};

        if (std::sqrt(
                std::pow(flag_pos.latitude_deg - pos.latitude_deg, 2) +
                std::pow(flag_pos.longitude_deg - pos.longitude_deg, 2)
                ) <= detection_radius) {
            this->mut.lock();
            this->flag_found = true;
            flag_found = true;
            flag_found_by_me = true;
            this->mut.unlock();
        } else {
            this->mut.lock();
            flag_found = this->flag_found;
            this->mut.unlock();
        }

        if (flag_found) {
            this->telemetry->subscribe_position(nullptr);

            this->callback(flag_pos, flag_found_by_me);
        }
    }
    );

    MCSSuccess success;
    return success;
}