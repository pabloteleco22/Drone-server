#include "missionhelper.hpp"
#include <iostream>

CannotMakeMission::CannotMakeMission(std::string message) {
    this->message = "CannotMakeMission: " + message;
}

const char *CannotMakeMission::what() const noexcept {
    return message.c_str();
}

MissionHelper::MissionHelper(shared_ptr<Polygon> area) {
    this->area = area;
}

Mission::MissionItem MissionHelper::make_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    Mission::MissionItem::CameraAction camera_action)
{
    Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.is_fly_through = is_fly_through;
    new_item.gimbal_pitch_deg = gimbal_pitch_deg;
    new_item.gimbal_yaw_deg = gimbal_yaw_deg;
    new_item.camera_action = camera_action;
    return new_item;
}

void ParallelSweep::new_mission(const unsigned int system_id, const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission) const {
    const double scale{1024.0};
    Polygon helper = *area;
    for (size_t i = 0; i < helper.size(); ++i) {
        helper[i] *= scale;
    }
    double partial_area{helper.count_square() / static_cast<double>(number_of_systems)};

    if (partial_area <= 0) {
        throw CannotMakeMission("The required area is zero or less");
    }

    if (system_id <= 0) {
        throw CannotMakeMission("The system ID must be greater than 0");
    }

    if (system_id > number_of_systems) {
        throw CannotMakeMission("The system ID must be less than or equal to the number of systems");
    }

    Polygon polygon_of_interest;
    Polygon discarded_area;
    Segment cut_line;

    unsigned int n_iterations{(system_id < number_of_systems) ? system_id : (number_of_systems - 1)};

    for (unsigned int i = 0; i < n_iterations; ++i) {
        if (not helper.split(partial_area, discarded_area, polygon_of_interest, cut_line)) {
            throw CannotMakeMission("Cannot split the required area");
        }

        helper = discarded_area;
    }

    if (number_of_systems == 1) {
        polygon_of_interest = *area;
    } else {
        if (system_id == number_of_systems) 
            polygon_of_interest = discarded_area;

        for (size_t i = 0; i < polygon_of_interest.size(); ++i) {
            polygon_of_interest[i] /= scale;
        }
    }

    mission.push_back(MissionHelper::make_mission_item(
        polygon_of_interest.find_center().x,
        polygon_of_interest.find_center().y,
        //47.398170327054473,
        //8.5456490218639658,
        10.0f,
        5.0f,
        false,
        20.0f,
        60.0f,
        Mission::MissionItem::CameraAction::None)
    );
}