/**
 * The MIT License (MIT)
 * Copyright (c) 2023 Pablo López Sedeño
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "missionhelper.hpp"

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
        throw CannotMakeMission{"The required area is zero or less"};
    }

    if (system_id <= 0) {
        throw CannotMakeMission{"The system ID must be greater than 0"};
    }

    if (system_id > number_of_systems) {
        throw CannotMakeMission{"The system ID must be less than or equal to the number of systems"};
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

unsigned int SpiralSweepCenter::auto_system_id{1};
std::mutex SpiralSweepCenter::mut{};

void SpiralSweepCenter::new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id) const {
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

    std::vector<Segment> segment_vector{};

    for (Point p : polygon_of_interest.get_vertices()) {
        segment_vector.push_back(Segment{center, p});
    }

    auto it{segment_vector.begin()};

    while (!segment_vector.empty()) {
        Point p{it->get_point_along(separation)};
        Point end{it->get_end()};

        if (p != end) {
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

            *it = Segment{p, end};
            ++it;
        } else {
            segment_vector.erase(it);
        }

        if (it == segment_vector.end())
            it = segment_vector.begin();
    }

    std::reverse(mission.begin(), mission.end());

    mission.push_back(MissionHelper::make_mission_item(
        center.x,
        center.y,
        altitude,
        5.0f,
        false,
        20.0f,
        60.0f,
        Mission::MissionItem::CameraAction::None)
    );
}

unsigned int SpiralSweepEdge::auto_system_id{1};
std::mutex SpiralSweepEdge::mut{};

void SpiralSweepEdge::new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id) const {
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

    std::vector<Segment> segment_vector{};

    for (Point p : polygon_of_interest.get_vertices()) {
        segment_vector.push_back(Segment{p, center});
    }

    auto it{segment_vector.begin()};

    while (!segment_vector.empty()) {
        Point p{it->get_point_along(separation)};

        if (p != center) {
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

            *it = Segment{p, center};
            ++it;
        } else {
            segment_vector.erase(it);
        }

        if (it == segment_vector.end())
            it = segment_vector.begin();
    }
}

unsigned int ParallelSweep::auto_system_id{1};
std::mutex ParallelSweep::mut{};

void ParallelSweep::new_mission(const unsigned int number_of_systems, std::vector<Mission::MissionItem> &mission, unsigned int system_id) const {
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

    const Vector dir{polygon_of_interest[1] - polygon_of_interest[0]};
    const Vector norm{dir.norm().unit() * separation};
    const Line base_line{polygon_of_interest[0], dir};
    std::function<void(const bool, const Vector)> sweep{
        [base_line, dir, polygon_of_interest, altitude, *this, &mission](const bool first, const Vector norm) {
            Line tmp{base_line};

            bool cont{true};
            bool alt{true};

            if (first)
                tmp = Line{tmp.get_p1() + norm, dir};

            do {
                std::vector<Point> cross_points{cross_point(polygon_of_interest, tmp)};

                tmp = Line{tmp.get_p1() + norm, dir};

                if (cross_points.size() == 0) {
                    cont = false;
                } else if (cross_points.size() == 1) {
                    mission.push_back(MissionHelper::make_mission_item(
                        cross_points[0].x,
                        cross_points[0].y,
                        altitude,
                        5.0f,
                        false,
                        20.0f,
                        60.0f,
                        Mission::MissionItem::CameraAction::None)
                    );
                } else {
                    double max{0};
                    double min{0};

                    for (unsigned int i = 1; i < cross_points.size(); ++i){
                        if ((cross_points[max].y < cross_points[i].y) or
                                ((cross_points[max].y == cross_points[i].y) and
                                (cross_points[max].x < cross_points[i].x))
                            ) {
                            max = i;
                        }

                        if ((cross_points[min].y > cross_points[i].y) or
                                ((cross_points[min].y == cross_points[i].y) and
                                (cross_points[min].x > cross_points[i].x))
                            ) {
                            min = i;
                        }
                    }

                    Point first{cross_points[alt ? max : min]};
                    Point second{cross_points[alt ? min : max]};

                    Vector side{Vector{second - first}.unit()};
                    
                    double distance{first.distance(second)};
                    if (distance > 2 * separation) {
                        first = first + (side * separation);
                        second = second + ((-side) * separation);
                    } else if (distance > separation) {
                        first = first + (side * separation);
                    }

                    mission.push_back(MissionHelper::make_mission_item(
                        first.x,
                        first.y,
                        altitude,
                        5.0f,
                        false,
                        20.0f,
                        60.0f,
                        Mission::MissionItem::CameraAction::None)
                    );

                    mission.push_back(MissionHelper::make_mission_item(
                        second.x,
                        second.y,
                        altitude,
                        5.0f,
                        false,
                        20.0f,
                        60.0f,
                        Mission::MissionItem::CameraAction::None)
                    );

                    alt = not alt;
                }
            } while (cont);
        }
    };

    sweep(true, norm);

    std::reverse(mission.begin(), mission.end());

    sweep(false, -norm);
}

std::vector<Point> ParallelSweep::cross_point(const Polygon &poly, const Line &l) const {
    size_t len{poly.size()};
    std::vector<Point> cross_points{};
    Point p;
    bool success;

    for (size_t i = 0; i < len; ++i) {
        Segment s{poly[i], poly[(i + 1) % len]};

        success = s.cross_line(l, p);

        if (success)
            cross_points.push_back(p);
    }

    return cross_points;
}