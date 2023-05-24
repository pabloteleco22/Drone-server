#include "missioncontrol.hpp"

#include <cmath>

SearchController::SearchController(std::shared_ptr<mavsdk::System> system,
                    const Flag *flag, std::function<void()> callback,
                    double position_rate, double separation) {
    this->system = system;
    this->telemetry = new mavsdk::Telemetry{system};
    this->flag = flag;
    this->callback = callback;
    this->position_rate = position_rate;
    this->separation = separation;
}

SearchController::~SearchController() {
    delete telemetry;
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
        if ((std::fabs(pos.latitude_deg - flag_pos.latitude_deg) < separation) and
        (std::fabs(pos.longitude_deg - flag_pos.longitude_deg) < separation)) {
            this->telemetry->subscribe_position(nullptr);
            this->callback();
        }
    }
    );

    if (result == mavsdk::Telemetry::Result::Success)
        return MissionControllerStatus::SUCCESS;
    else
        return MissionControllerStatus::FAILURE;
}