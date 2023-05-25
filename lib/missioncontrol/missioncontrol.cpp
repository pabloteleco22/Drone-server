#include "missioncontrol.hpp"

#include <cmath>

bool SearchController::flag_found{false};
std::mutex SearchController::mut{};

SearchController::SearchController(mavsdk::Telemetry *telemetry, mavsdk::Action *action,
                    const Flag *flag, std::function<void(Flag::Position, bool)> callback,
                    double position_rate, double separation) {
    this->telemetry = telemetry;
    this->action = action;
    this->flag = flag;
    this->callback = callback;
    this->position_rate = position_rate;
    this->separation = separation;
}

MissionControllerStatus SearchController::mission_control() {
    // Establecer a un segundo la frecuencia de transmisión de la posición
    mavsdk::Telemetry::Result result{telemetry->set_rate_position(position_rate)};
    // Pedir regularmente la posición de los drones
    telemetry->subscribe_position(
        [this](mavsdk::Telemetry::Position pos) {
        Flag::Position flag_pos{this->flag->get_flag_position()};
        bool flag_found;
        bool flag_found_by_me{false};

        if ((std::fabs(pos.latitude_deg - flag_pos.latitude_deg) < separation) and
        (std::fabs(pos.longitude_deg - flag_pos.longitude_deg) < separation)) {
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

            this->action->return_to_launch_async([](mavsdk::Action::Result) {});

            this->callback(flag_pos, flag_found_by_me);
        }
    }
    );

    if (result == mavsdk::Telemetry::Result::Success)
        return MissionControllerStatus::SUCCESS;
    else
        return MissionControllerStatus::FAILURE;
}