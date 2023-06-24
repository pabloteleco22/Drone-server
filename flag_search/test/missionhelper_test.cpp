/** Copyright (C) 2023  Pablo López Sedeño
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>
#include "../src/missionhelper/missionhelper.hpp"

TEST(GoCenterTest, NewMissionThrowException) {
    Polygon poly;
    MissionHelper *mission_helper{new GoCenter{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_THROW(mission_helper->new_mission(3, mission_item_list, 2), CannotMakeMission);

    delete mission_helper;
}

TEST(GoCenterTest, NewMission) {
    Polygon poly;
    poly.push_back({0,0});
    poly.push_back({1,0});
    poly.push_back({1,1});
    poly.push_back({0,1});

    MissionHelper *mission_helper{new GoCenter{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(GoCenterTest, NewMission2) {
    Polygon poly;
    poly.push_back({0,0});
    poly.push_back({100,0});
    poly.push_back({100,20});
    poly.push_back({0,20});

    MissionHelper *mission_helper{new GoCenter{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(GoCenterTest, NewMission3) {
    Polygon poly;
    poly.push_back({47.397841,8.545629});
    poly.push_back({47.397841,8.546824});
    poly.push_back({47.398021,8.546824});
    poly.push_back({47.398021,8.545629});

    MissionHelper *mission_helper{new GoCenter{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(GoCenterTest, NewMission4) {
    Polygon poly;
    poly.push_back({47.3978,8.5456});
    poly.push_back({47.3978,8.5468});
    poly.push_back({47.3980,8.5468});
    poly.push_back({47.3980,8.5456});

    MissionHelper *mission_helper{new GoCenter{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(SpiralSweepCenterTest, NewMission1) {
    Polygon poly;
    poly.push_back({47.3978,8.5456});
    poly.push_back({47.3978,8.5468});
    poly.push_back({47.3980,8.5468});
    poly.push_back({47.3980,8.5456});

    MissionHelper *mission_helper{new SpiralSweepCenter{poly, 0.000018}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(SpiralSweepCenterTest, NewMission2) {
    Polygon poly;
    poly.push_back({});
    poly.push_back({0, 90});
    poly.push_back({20, 90});
    poly.push_back({20, 0});

    MissionHelper *mission_helper{new SpiralSweepCenter{poly, 5}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(SpiralSweepEdgeTest, NewMission1) {
    Polygon poly;
    poly.push_back({47.3978,8.5456});
    poly.push_back({47.3978,8.5468});
    poly.push_back({47.3980,8.5468});
    poly.push_back({47.3980,8.5456});

    MissionHelper *mission_helper{new SpiralSweepEdge{poly, 0.000018}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(SpiralSweepEdgeTest, NewMission2) {
    Polygon poly;
    poly.push_back({});
    poly.push_back({0, 90});
    poly.push_back({20, 90});
    poly.push_back({20, 0});

    MissionHelper *mission_helper{new SpiralSweepEdge{poly, 5}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(ParallelSweep, NewMission) {
    Polygon poly;
    poly.push_back({});
    poly.push_back({0, 90});
    poly.push_back({20, 90});
    poly.push_back({20, 0});

    MissionHelper *mission_helper{new ParallelSweep{poly, 1}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}

TEST(ParallelSweep, NewMission2) {
    Polygon poly;
    poly.push_back({-20, 10});
	poly.push_back({-10, 30});
	poly.push_back({7.5, 30});
	poly.push_back({15, 10});
	poly.push_back({22.5, 20});
	poly.push_back({30, -10});
	poly.push_back({-25, -10});
	poly.push_back({-40, -5});
	poly.push_back({-30, 20});

    MissionHelper *mission_helper{new ParallelSweep{poly, 5}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 1));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 2));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 3));
    ASSERT_NO_THROW(mission_helper->new_mission(4, mission_item_list, 4));

    delete mission_helper;
}