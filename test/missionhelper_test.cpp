#include <gtest/gtest.h>
#include "../lib/missionhelper/missionhelper.hpp"

TEST(ParallelSweepTest, NewMissionThrowException) {
    shared_ptr<Polygon> poly{new Polygon};
    shared_ptr<MissionHelper> mission_helper{new ParallelSweep{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_THROW(mission_helper->new_mission(2, 3, mission_item_list), CannotMakeMission);
}

TEST(ParallelSweepTest, NewMission) {
    shared_ptr<Polygon> poly{new Polygon};
    poly->push_back({0,0});
    poly->push_back({1,0});
    poly->push_back({1,1});
    poly->push_back({0,1});

    shared_ptr<MissionHelper> mission_helper{new ParallelSweep{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(2, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(3, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(4, 4, mission_item_list));
}

TEST(ParallelSweepTest, NewMission2) {
    shared_ptr<Polygon> poly{new Polygon};
    poly->push_back({0,0});
    poly->push_back({100,0});
    poly->push_back({100,20});
    poly->push_back({0,20});

    shared_ptr<MissionHelper> mission_helper{new ParallelSweep{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, 1, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(1, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(2, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(3, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(4, 4, mission_item_list));
}

TEST(ParallelSweepTest, NewMission3) {
    shared_ptr<Polygon> poly{new Polygon};
    poly->push_back({47.397841,8.545629});
    poly->push_back({47.397841,8.546824});
    poly->push_back({47.398021,8.546824});
    poly->push_back({47.398021,8.545629});

    shared_ptr<MissionHelper> mission_helper{new ParallelSweep{poly}};
    std::vector<Mission::MissionItem> mission_item_list;

    ASSERT_NO_THROW(mission_helper->new_mission(1, 1, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(1, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(1, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(2, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(3, 4, mission_item_list));
    ASSERT_NO_THROW(mission_helper->new_mission(4, 4, mission_item_list));
}