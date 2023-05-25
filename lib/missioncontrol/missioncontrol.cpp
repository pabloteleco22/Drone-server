#include "missioncontrol.hpp"

#include <cmath>

bool SearchController::flag_found{false};
std::mutex SearchController::mut{};

SearchController::SearchController(mavsdk::Telemetry *telemetry, mavsdk::Action *action,
                    const Flag *flag, std::function<void()> callback,
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
        // Si la posición está lo suficientemente cerca de la bandera
        // se llama al callback
        Flag::Position flag_pos{this->flag->get_flag_position()};
        bool flag_found;
        if ((std::fabs(pos.latitude_deg - flag_pos.latitude_deg) < separation) and
        (std::fabs(pos.longitude_deg - flag_pos.longitude_deg) < separation)) {
            this->callback();
            
            this->mut.lock();
            this->flag_found = true;
            flag_found = true;
            this->mut.unlock();
        } else {
            this->mut.lock();
            flag_found = this->flag_found;
            this->mut.unlock();
        }

        //cout << "System " << std::to_string(this->system->get_system_id()) << " " << std::boolalpha << flag_found << endl;
        if (flag_found) {
            this->telemetry->subscribe_position(nullptr);

            this->action->return_to_launch_async([](mavsdk::Action::Result) {});
        }
    }
    );

    if (result == mavsdk::Telemetry::Result::Success)
        return MissionControllerStatus::SUCCESS;
    else
        return MissionControllerStatus::FAILURE;
}