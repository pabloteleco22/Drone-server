#include "missionhelper.hpp"

CannotMakeMission::CannotMakeMission(std::string message) {
    this->message = "CannotMakeMission: " + message;
}

const char *CannotMakeMission::what() const noexcept {
    return message.c_str();
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

PolySplitMission::PolySplitMission(Polygon area) {
    this->area = area;
}

void PolySplitMission::get_polygon_of_interest(const unsigned int system_id, const unsigned int number_of_systems, Polygon *polygon_of_interest) const {
    const double precision{1E6};
    Polygon helper = area;
    for (size_t i = 0; i < helper.size(); ++i) {
        helper[i] *= precision;
        helper[i].x = round(helper[i].x);
        helper[i].y = round(helper[i].y);
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

    Polygon poly1;
    Polygon poly2;
    Polygon *polygon_of_interest_tmp = &poly1;
    Polygon *discarded_area{&poly2};
    Segment cut_line;

    const unsigned int n_iterations{(system_id < number_of_systems) ? system_id : (number_of_systems - 1)};

    for (unsigned int i = 0; i < n_iterations; ++i) {
        try {
            helper.split(partial_area, poly1, poly2, cut_line);
        } catch (const Polygon::CannotSplitException &e) {
            throw CannotMakeMission(std::string{"Cannot split the required area. "} + e.what());
        }

        if (poly1.count_square() - partial_area < poly2.count_square() - partial_area) {
            polygon_of_interest_tmp = &poly1;
            discarded_area = &poly2;
        } else {
            polygon_of_interest_tmp = &poly2;
            discarded_area = &poly1;
        }

        helper = *discarded_area;
    }

    if (number_of_systems == 1) {
        *polygon_of_interest_tmp = area;
    } else {
        if (system_id == number_of_systems) 
            polygon_of_interest_tmp = discarded_area;

        for (size_t i = 0; i < polygon_of_interest_tmp->size(); ++i) {
            (*polygon_of_interest_tmp)[i] /= precision;
        }
    }

    *polygon_of_interest = *polygon_of_interest_tmp;
}

void GoCenter::new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id) const {
    Polygon polygon_of_interest;

    get_polygon_of_interest(system_id, number_of_systems, &polygon_of_interest);

    float altitude{static_cast<float>(system_id)};
    const float altitude_offset{10.0f};
    altitude += altitude_offset;

    mission.push_back(MissionHelper::make_mission_item(
        polygon_of_interest.find_center().x,
        polygon_of_interest.find_center().y,
        altitude,
        5.0f,
        false,
        20.0f,
        60.0f,
        Mission::MissionItem::CameraAction::None)
    );
}

unsigned int SpiralSweep::auto_system_id{1};
std::mutex SpiralSweep::mut{};

void SpiralSweep::new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id) const {
    Polygon polygon_of_interest;

    if (system_id > 255) {
        mut.lock();
        system_id = auto_system_id;
        ++auto_system_id;
        if (auto_system_id > number_of_systems) auto_system_id = 1;
        mut.unlock();
    }

    get_polygon_of_interest(system_id, number_of_systems, &polygon_of_interest);

    float altitude{static_cast<float>(system_id)};
    const float altitude_offset{10.0f};
    altitude += altitude_offset;

    const Point center{polygon_of_interest.find_center()};

    // Cogemos el primer vértice del polígono
    std::vector<Segment> segment_vector{};

    for (Point p : polygon_of_interest.get_vertex()) {
        segment_vector.push_back(Segment{p, center});
    }

    auto it{segment_vector.begin()};

    while (!segment_vector.empty()) {
        Point p{it->get_point_along(separation)};

        mission.push_back(MissionHelper::make_mission_item(
            p.x,
            p.y,
            altitude,
            5.0f,
            false,
            20.0f,
            60.0f,
            Mission::MissionItem::CameraAction::None)
        );

        if (p == center) {
            segment_vector.erase(it);
        } else {
            *it = Segment{p, center};
            ++it;
        }

        if (it == segment_vector.end())
            it = segment_vector.begin();
    }
}