#include <gtest/gtest.h>

#include <cmath>

#include "../lib/poly/point.hpp"
#include "../lib/poly/vector.hpp"
#include "../lib/poly/line.hpp"
#include "../lib/poly/polygon.hpp"

/* Point Tests */
TEST(PointTest, DefaultPoint) {
    const Point point;
    ASSERT_EQ(point.x, 0);
    ASSERT_EQ(point.y, 0);
}

TEST(PointTest, CustomPoint) {
    const double expected_x{1.3};
    const double expected_y{3.7};

    const Point point{expected_x, expected_y};

    ASSERT_EQ(point.x, expected_x);
    ASSERT_EQ(point.y, expected_y);
}

TEST(PointTest, Neg) {
    const double expected_x{1.3};
    const double expected_y{3.7};

    const Point point{expected_x, expected_y};
    const Point neg{-point};

    ASSERT_EQ(neg.x, -expected_x);
    ASSERT_EQ(neg.y, -expected_y);
}

TEST(PointTest, ChangingPoint) {
    Point point;

    ASSERT_EQ(point.x, 0);
    ASSERT_EQ(point.y, 0);

    const double expected_x{1.3};
    const double expected_y{3.7};

    point.x = expected_x;
    point.y = expected_y;

    ASSERT_EQ(point.x, expected_x);
    ASSERT_EQ(point.y, expected_y);
}

TEST(PointTest, Length) {
    const double x1{1.3};
    const double y1{3.7};
    const double x2{9.4};
    const double y2{-4.0};
    const double expected_length{11.17586686};

    const Point p1{x1, y1};
    const Point p2{x2, y2};

    ASSERT_LE(p1.distance(p2) - expected_length, POLY_SPLIT_EPS);
}

TEST(PointTest, Absolute) {
    const double x{-1.3};
    const double y{3.7};
    const double expected_x{-x};
    const double expected_y{y};

    const Point point{x, y};
    const Point expected_point{expected_x, expected_y};

    ASSERT_EQ(point.abs(), expected_point);
}

/* Vector Tests */
TEST(VectorTest, DefaultVector) {
    const Vector vec;
    ASSERT_EQ(vec.x, 0);
    ASSERT_EQ(vec.y, 0);
}

TEST(VectorTest, CustomVector) {
    const double expected_x{1.3};
    const double expected_y{3.7};

    const Vector vec{expected_x, expected_y};

    ASSERT_EQ(vec.x, expected_x);
    ASSERT_EQ(vec.y, expected_y);
}

TEST(VectorTest, Neg) {
    const double expected_x{1.3};
    const double expected_y{3.7};

    const Vector vec{expected_x, expected_y};
    const Vector neg{-vec};

    ASSERT_EQ(neg.x, -expected_x);
    ASSERT_EQ(neg.y, -expected_y);
}

TEST(VectorTest, Length) {
    const double x{1.3};
    const double y{3.7};
    const double expected_length{sqrt(x*x + y*y)};

    const Vector vec{x, y};

    ASSERT_EQ(vec.length(), expected_length);
}

TEST(VectorTest, Absolute) {
    const double x{-1.3};
    const double y{3.7};
    const double expected_x{-x};
    const double expected_y{y};

    const Vector vec{x, y};
    const Vector expected_vec{expected_x, expected_y};

    ASSERT_EQ(vec.abs(), expected_vec);
}

/* Line Tests */
TEST(LineTest, DefaultLine) {
    const Line lin;
    const Point expected_point{0, 0};

    ASSERT_EQ(lin.get_p1(), expected_point);
    ASSERT_EQ(lin.get_p2(), expected_point);
}

TEST(LineTest, CustomLine) {
    const Point start_point{5, 8};
    const Point end_point{8.9, 3};
    const Line lin{start_point, end_point};

    ASSERT_EQ(lin.get_p1(), start_point);
    ASSERT_EQ(lin.get_p2(), end_point);
}

TEST(LineTest, PointAlong) {
    const Point start_point;
    const Point end_point{25, 25};
    const Line lin{start_point, end_point};

    const Point expected_point{8.48528137423857, 8.48528137423857};
    const Point result = lin.get_point_along(12);

    ASSERT_EQ(result, expected_point);
}

TEST(LineTest, DistanceIn) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{2, 4};
    const Line lin{start_point, end_point};

    const double expected_distance{1.414213562};

    ASSERT_LE(fabs(fabs(lin.get_distance(distance_point)) - expected_distance), POLY_SPLIT_EPS);
}

TEST(LineTest, DistanceOut) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-2, -4};
    const Line lin{start_point, end_point};

    const double expected_distance{1.414213562};

    ASSERT_LE(fabs(fabs(lin.get_distance(distance_point)) - expected_distance), POLY_SPLIT_EPS);
}

TEST(LineTest, LineNearestPointIn) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{2, 4};
    const Line lin{start_point, end_point};
    const Point expected_point{3, 3};

    ASSERT_EQ(lin.get_nearest_point(distance_point), expected_point);
}

TEST(LineTest, LineNearestPointOut) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-2, -4};
    const Line lin{start_point, end_point};
    const Point expected_point{-3, -3};

    ASSERT_EQ(lin.get_nearest_point(distance_point), expected_point);
}

TEST(LineTest, PointSideAbove) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{2, 4};
    const Line lin{start_point, end_point};
    const PointSide expected_result{PointSide::Above};

    ASSERT_EQ(lin.point_side(distance_point), expected_result);
}

TEST(LineTest, PointSideBelow) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-2, -4};
    const Line lin{start_point, end_point};
    const PointSide expected_result{PointSide::Below};

    ASSERT_EQ(lin.point_side(distance_point), expected_result);
}

TEST(LineTest, PointSideInside) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-1, -1};
    const Line lin{start_point, end_point};
    const PointSide expected_result{PointSide::Inside};

    ASSERT_EQ(lin.point_side(distance_point), expected_result);
}

TEST(LineTest, CrossLineLineTrue) {
    const Point start_point1;
    const Point end_point1{4, 4};
    const Point start_point2{0, 4};
    const Point end_point2{6, 0};
    Point inter_point;
    const Point expected_inter_point{2.4, 2.4};
    const Line lin1{start_point1, end_point1};
    const Line lin2{start_point2, end_point2};

    ASSERT_TRUE(lin1.cross_line(lin2, inter_point));
    ASSERT_EQ(inter_point, expected_inter_point);
}

TEST(LineTest, CrossLineLineFalse) {
    const Point start_point1;
    const Point end_point1{4, 4};
    const Point start_point2{0, 4};
    const Point end_point2{-4, 0};
    Point inter_point;
    const Line lin1{start_point1, end_point1};
    const Line lin2{start_point2, end_point2};

    ASSERT_FALSE(lin1.cross_line(lin2, inter_point));
}

TEST(LineTest, CrossLineSegmentTrue) {
    const Point line_start_point;
    const Point line_end_point{4, 4};
    const Point segment_start_point{0, 4};
    const Point segment_end_point{6, 0};
    Point inter_point;
    const Point expected_inter_point{2.4, 2.4};
    const Line line{line_start_point, line_end_point};
    const Segment segment{segment_start_point, segment_end_point};

    ASSERT_TRUE(cross_line(line, segment, inter_point));
    ASSERT_EQ(inter_point, expected_inter_point);
}

TEST(LineTest, CrossLineSegmentFalse) {
    const Point line_start_point{0, 6};
    const Point line_end_point{18, 0};
    const Point segment_start_point;
    const Point segment_end_point{4, 4};
    Point inter_point;
    const Line line{line_start_point, line_end_point};
    const Segment segment{segment_start_point, segment_end_point};

    ASSERT_FALSE(cross_line(line, segment, inter_point));
}

TEST(LineTest, IsSameTrue1) {
    const Point start_point1;
    const Point end_point1{2, 4};
    const Point start_point2;
    const Point end_point2{2, 4};
    const Line line1{start_point1, end_point1};
    const Line line2{start_point2, end_point2};

    ASSERT_TRUE(Line::is_same(line1, line2));
}

TEST(LineTest, IsSameTrue2) {
    const Point start_point1;
    const Point end_point1{2, 4};
    const Point start_point2;
    const Point end_point2{4, 8};
    const Line line1{start_point1, end_point1};
    const Line line2{start_point2, end_point2};

    ASSERT_TRUE(Line::is_same(line1, line2));
}

TEST(LineTest, IsSameFalse) {
    const Point start_point1;
    const Point end_point1{2, 4};
    const Point start_point2;
    const Point end_point2{4, 8.1};
    const Line line1{start_point1, end_point1};
    const Line line2{start_point2, end_point2};

    ASSERT_FALSE(Line::is_same(line1, line2));
}

TEST(LineTest, Bisector1) {
    const Point start_point1;
    const Point end_point1{0, 1};
    const Point start_point2;
    const Point end_point2{1, 0};
    const Line line1{start_point1, end_point1};
    const Line line2{start_point2, end_point2};

    const Point expected_start_point;
    const Point expected_end_point{1, -1};
    const Line expected_bisector{expected_start_point, expected_end_point};

    const Line returned = Line::get_bisector(line1, line2);
    Point inter;

    ASSERT_FALSE(returned.cross_line(expected_bisector, inter));
    ASSERT_EQ(returned.get_distance(expected_end_point), 0);
}

TEST(LineTest, Bisector2) {
    const Point start_point;
    const Point end_point{0, 1};
    const Line line{start_point, end_point};

    const Point expected_start_point;
    const Point expected_end_point{end_point};
    const Line expected_bisector{expected_start_point, expected_end_point};

    const Line returned = Line::get_bisector(line, line);
    Point inter;

    ASSERT_FALSE(returned.cross_line(expected_bisector, inter));
    ASSERT_EQ(returned.get_distance(expected_end_point), 0);
}

TEST(LineTest, TanAngleZero) {
    const Point start_point;
    const Point end_point{0, 1};
    const Line line{start_point, end_point};

    const double expected_tan{0};

    ASSERT_EQ(Line::get_tan_angle(line, line), expected_tan);
}

TEST(LineTest, TanAngle1) {
    const Point start_point1;
    const Point end_point1{0, 1};
    const Point start_point2;
    const Point end_point2{1, 1};
    const Line line1{start_point1, end_point1};
    const Line line2{start_point2, end_point2};

    const double expected_tan{-1};

    ASSERT_EQ(Line::get_tan_angle(line1, line2), expected_tan);
}

TEST(LineTest, DirectedLine) {
    const Point point{1, 2};
    const Point vector{1, 1};
    const Line result_line = Line::directed_line(point, vector);
    const Point expected_start_point{1, 2};
    const Point expected_end_point{4.5, 5.5};
    const Line expected_line{expected_start_point, expected_end_point};

    ASSERT_TRUE(Line::is_same(result_line, expected_line));
}

/* Segment Tests */
TEST(SegmentTest, DefaultSegment) {
    const Segment seg;
    const Point expected_point{0, 0};

    ASSERT_EQ(seg.get_start(), expected_point);
    ASSERT_EQ(seg.get_end(), expected_point);
}

TEST(SegmentTest, CustomSegmentPoints) {
    const Point start_point{5, 8};
    const Point end_point{8.9, 3};
    const Segment seg{start_point, end_point};

    ASSERT_EQ(seg.get_start(), start_point);
    ASSERT_EQ(seg.get_end(), end_point);
}

TEST(SegmentTest, CustomSegmentLine) {
    const Point start_point{5, 8};
    const Point end_point{8.9, 3};
    const Line lin{start_point, end_point};
    const Segment seg{lin};

    ASSERT_EQ(seg.get_start(), start_point);
    ASSERT_EQ(seg.get_end(), end_point);
}

TEST(SegmentTest, reverse) {
    const Point start_point{5, 8};
    const Point end_point{8.9, 3};
    const Segment seg{start_point, end_point};

    ASSERT_EQ(seg.reverse().get_start(), end_point);
    ASSERT_EQ(seg.reverse().get_end(), start_point);
}

TEST(SegmentTest, Length) {
    const Point start_point{5, 8};
    const Point end_point{8, 3};
    const Segment seg{start_point, end_point};

    const double expected_distance{end_point.distance(start_point)};

    ASSERT_EQ(seg.length(), expected_distance);
}

TEST(SegmentTest, PointAlong) {
    const Point start_point;
    const Point end_point{25, 25};
    const Segment seg{start_point, end_point};

    const Point expected_point{8.48528137423857, 8.48528137423857};
    const Point result = seg.get_point_along(12);

    ASSERT_EQ(result, expected_point);
}

TEST(SegmentTest, DistanceIn) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{2, 4};
    const Segment seg{start_point, end_point};

    const double expected_distance{1.414213562};

    ASSERT_LE(fabs(fabs(seg.get_distance(distance_point)) - expected_distance), POLY_SPLIT_EPS);
}

TEST(SegmentTest, DistanceOut) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-2, -4};
    const Segment seg{start_point, end_point};

    const double expected_distance{1.414213562};

    ASSERT_LE(fabs(fabs(seg.get_distance(distance_point)) - expected_distance), POLY_SPLIT_EPS);
}

TEST(SegmentTest, SegmentNearestPointIn) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{2, 4};
    const Segment seg{start_point, end_point};
    const Point expected_point{3, 3};

    ASSERT_EQ(seg.get_nearest_point(distance_point), expected_point);
}

TEST(SegmentTest, SegmentNearestPointOut) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-2, -4};
    const Segment seg{start_point, end_point};
    const Point expected_point{0, 0};

    ASSERT_EQ(seg.get_nearest_point(distance_point), expected_point);
}

TEST(SegmentTest, PointSideAbove) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{2, 4};
    const Segment seg{start_point, end_point};
    const PointSide expected_result{PointSide::Above};

    ASSERT_EQ(seg.point_side(distance_point), expected_result);
}

TEST(SegmentTest, PointSideBelow) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-2, -4};
    const Segment seg{start_point, end_point};
    const PointSide expected_result{PointSide::Below};

    ASSERT_EQ(seg.point_side(distance_point), expected_result);
}

TEST(SegmentTest, PointSideInside) {
    const Point start_point;
    const Point end_point{10, 10};
    const Point distance_point{-1, -1};
    const Segment seg{start_point, end_point};
    const PointSide expected_result{PointSide::Inside};

    ASSERT_EQ(seg.point_side(distance_point), expected_result);
}

TEST(SegmentTest, CrossSegmentLineTrue) {
    const Point line_start_point;
    const Point line_end_point{4, 4};
    const Point segment_start_point{0, 4};
    const Point segment_end_point{6, 0};
    Point inter_point;
    const Point expected_inter_point{2.4, 2.4};
    const Line line{line_start_point, line_end_point};
    const Segment segment{segment_start_point, segment_end_point};

    ASSERT_TRUE(segment.cross_line(line, inter_point));
    ASSERT_EQ(inter_point, expected_inter_point);
}

TEST(SegmentTest, CrossSegmentLineFalse) {
    const Point line_start_point{0, 6};
    const Point line_end_point{18, 0};
    const Point segment_start_point;
    const Point segment_end_point{4, 4};
    Point inter_point;
    const Line line{line_start_point, line_end_point};
    const Segment segment{segment_start_point, segment_end_point};

    ASSERT_FALSE(segment.cross_line(line, inter_point));
}

TEST(SegmentTest, CrossSegmentSegmentTrue) {
    const Point start_point1;
    const Point end_point1{4, 4};
    const Point start_point2{0, 4};
    const Point end_point2{6, 0};
    Point inter_point;
    const Point expected_inter_point{2.4, 2.4};
    const Segment segment1{start_point1, end_point1};
    const Segment segment2{start_point2, end_point2};

    ASSERT_TRUE(segment1.cross_line(segment2, inter_point));
    ASSERT_EQ(inter_point, expected_inter_point);
}

TEST(SegmentTest, CrossSegmentSegmentFalse1) {
    const Point start_point1;
    const Point end_point1{4, 4};
    const Point start_point2{0, 6};
    const Point end_point2{18, 0};
    Point inter_point;
    const Point expected_inter_point{2.4, 2.4};
    const Segment segment1{start_point1, end_point1};
    const Segment segment2{start_point2, end_point2};

    ASSERT_FALSE(segment1.cross_line(segment2, inter_point));
}

TEST(SegmentTest, CrossSegmentSegmentFalse2) {
    const Point start_point1{0, 6};
    const Point end_point1{18, 0};
    const Point start_point2;
    const Point end_point2{4, 4};
    Point inter_point;
    const Point expected_inter_point{2.4, 2.4};
    const Segment segment1{start_point1, end_point1};
    const Segment segment2{start_point2, end_point2};

    ASSERT_FALSE(segment1.cross_line(segment2, inter_point));
}

TEST(SegmentTest, IsSameTrue) {
    const Point start_point1;
    const Point end_point1{2, 4};
    const Point start_point2;
    const Point end_point2{2, 4};
    const Segment seg1{start_point1, end_point1};
    const Segment seg2{start_point2, end_point2};

    ASSERT_TRUE(Segment::is_same(seg1, seg2));
}

TEST(SegmentTest, IsSameFalse1) {
    const Point start_point1;
    const Point end_point1{2, 4};
    const Point start_point2;
    const Point end_point2{4, 8};
    const Segment seg1{start_point1, end_point1};
    const Segment seg2{start_point2, end_point2};

    ASSERT_FALSE(Segment::is_same(seg1, seg2));
}

TEST(SegmentTest, IsSameFalse2) {
    const Point start_point1;
    const Point end_point1{2, 4};
    const Point start_point2;
    const Point end_point2{4, 8.1};
    const Segment seg1{start_point1, end_point1};
    const Segment seg2{start_point2, end_point2};

    ASSERT_FALSE(Segment::is_same(seg1, seg2));
}

TEST(SegmentTest, Bisector1) {
    const Point start_point1;
    const Point end_point1{0, 1};
    const Point start_point2;
    const Point end_point2{1, 0};
    const Segment seg1{start_point1, end_point1};
    const Segment seg2{start_point2, end_point2};

    const Point expected_start_point;
    const Point expected_end_point{1, -1};
    const Line expected_bisector{expected_start_point, expected_end_point};

    const Line returned = Segment::get_bisector(seg1, seg2);
    Point inter;

    ASSERT_FALSE(returned.cross_line(expected_bisector, inter));
    ASSERT_EQ(returned.get_distance(expected_end_point), 0);
}

TEST(SegmentTest, Bisector2) {
    const Point start_point;
    const Point end_point{0, 1};
    const Segment segment{start_point, end_point};

    const Point expected_start_point;
    const Point expected_end_point{end_point};
    const Line expected_bisector{expected_start_point, expected_end_point};

    const Line returned = Segment::get_bisector(segment, segment);
    Point inter;

    ASSERT_FALSE(returned.cross_line(expected_bisector, inter));
    ASSERT_EQ(returned.get_distance(expected_end_point), 0);
}

TEST(SegmentTest, TanAngleZero) {
    const Point start_point;
    const Point end_point{0, 1};
    const Segment seg{start_point, end_point};

    const double expected_tan{0};

    ASSERT_EQ(Segment::get_tan_angle(seg, seg), expected_tan);
}

TEST(SegmentTest, TanAngle1) {
    const Point start_point1;
    const Point end_point1{0, 1};
    const Point start_point2;
    const Point end_point2{1, 1};
    const Segment seg1{start_point1, end_point1};
    const Segment seg2{start_point2, end_point2};

    const double expected_tan{-1};

    ASSERT_EQ(Segment::get_tan_angle(seg1, seg2), expected_tan);
}

/* Polygon Tests */
TEST(PolygonTest, ChangingPoint) {
    Point p1{2, 0};
    Points pol_points;
    pol_points.push_back(Point{});
    pol_points.push_back(p1);
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{0, 2});
    Polygon pol{pol_points};

    ASSERT_EQ(pol[1], p1);

    Point new_p1{5, 8};

    pol[1].x = new_p1.x;
    pol[1].y = new_p1.y;

    ASSERT_EQ(pol[1], new_p1);
}

TEST(PolygonTest, CountSquare) {
    Points pol_points;
    pol_points.push_back(Point{});
    pol_points.push_back(Point{2, 0});
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{0, 2});
    const Polygon pol{pol_points};

    const double expected_sqrt{4};

    ASSERT_EQ(pol.count_square(), expected_sqrt);
}

TEST(PolygonTest, SplitTrue) {
    Points original_points;
    original_points.push_back(Point{});
    original_points.push_back(Point{2, 0});
    original_points.push_back(Point{2, 2});
    original_points.push_back(Point{0, 2});
    const Polygon original_poly{original_points};
    Polygon first_poly;
    Polygon second_poly;
    Segment cut_line;
    const double expected_area{3};
    const Segment expected_cut_line{Point{1.5, 2}, Point{1.5, 0}};

    ASSERT_TRUE(original_poly.split(expected_area, first_poly, second_poly, cut_line));
    ASSERT_EQ(second_poly.count_square(), expected_area);
    ASSERT_EQ(first_poly.count_square() + second_poly.count_square(), original_poly.count_square());
    ASSERT_EQ(cut_line, expected_cut_line);
}

TEST(PolygonTest, SplitFalse) {
    Points original_points;
    original_points.push_back(Point{});
    original_points.push_back(Point{2, 0});
    original_points.push_back(Point{2, 2});
    original_points.push_back(Point{0, 2});
    const Polygon original_poly{original_points};
    Polygon first_poly;
    Polygon second_poly;
    Segment cut_line;
    const double expected_area{300};

    ASSERT_FALSE(original_poly.split(expected_area, first_poly, second_poly, cut_line));
}

TEST(PolygonTest, FindDistanceOutside) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    const Polygon poly{points};

    const Point point{2, 3};
    const double expected_distance{1};

    ASSERT_EQ(poly.find_distance(point), expected_distance);
}

TEST(PolygonTest, FindDistanceInside) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    const Polygon poly{points};

    const Point point{1, 1};
    const double expected_distance{1};

    ASSERT_EQ(poly.find_distance(point), expected_distance);
}

TEST(PolygonTest, FindEmptyDistance) {
    Points points;
    const Polygon poly{points};

    const Point point{2, 3};

    ASSERT_THROW(poly.find_distance(point), Polygon::NotEnoughPointsException);
}

TEST(PolygonTest, FindNearestPointOutside) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    const Polygon poly{points};

    const Point point{3, 3};
    const Point expected_point{2, 2};

    ASSERT_EQ(poly.find_nearest_point(point), expected_point);
}

TEST(PolygonTest, FindNearestPointInside) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    const Polygon poly{points};

    const Point point{1, 1.1};
    const Point expected_point{1, 2};

    ASSERT_EQ(poly.find_nearest_point(point), expected_point);
}

TEST(PolygonTest, FindEmptyNearestPoint) {
    Points points;
    const Polygon poly{points};

    const Point point{3, 3};

    ASSERT_THROW(poly.find_nearest_point(point), Polygon::NotEnoughPointsException);
}

TEST(PolygonTest, FindCenter) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    const Polygon poly{points};

    const Point expected_point{1, 1};

    ASSERT_EQ(poly.count_center(), expected_point);
}

TEST(PolygonTest, FindCenterEmpty) {
    Points points;
    const Polygon poly{points};

    const Point expected_point{};

    ASSERT_THROW(poly.count_center(), Polygon::NotEnoughPointsException);
}

TEST(PolygonTest, SplitNearestEdge) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    Polygon poly{points};

    const Point split_point{4, 1};
    const Point expected_point{2, 1};
    const size_t expected_size{5};

    poly.split_nearest_edge(split_point);

    ASSERT_EQ(poly[2], expected_point);
    ASSERT_EQ(poly.size(), expected_size);
}

TEST(PolygonTest, SplitNearestEdgeCorner) {
    Points points;
    points.push_back(Point{});
    points.push_back(Point{2, 0});
    points.push_back(Point{2, 2});
    points.push_back(Point{0, 2});
    Polygon poly{points};

    const Point split_point{4, 4};
    const size_t expected_size{4};

    poly.split_nearest_edge(split_point);

    ASSERT_EQ(poly.size(), expected_size);
}

TEST(PolygonTest, IsPointInsideTrue1) {
    Points pol_points;
    pol_points.push_back(Point{});
    pol_points.push_back(Point{2, 0});
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{0, 2});
    const Polygon pol{pol_points};

    const Point point{1, 1};

    ASSERT_TRUE(pol.is_point_inside(point));
}

TEST(PolygonTest, IsPointInsideTrue2) {
    Points pol_points;
    pol_points.push_back(Point{});
    pol_points.push_back(Point{2, 0});
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{0, 2});
    const Polygon pol{pol_points};

    const Point point{2, 1};

    ASSERT_TRUE(pol.is_point_inside(point));
}

TEST(PolygonTest, IsPointInsideFalse) {
    Points pol_points;
    pol_points.push_back(Point{});
    pol_points.push_back(Point{2, 0});
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{0, 2});
    const Polygon pol{pol_points};

    const Point point{-1, 1};

    ASSERT_FALSE(pol.is_point_inside(point));
}

TEST(PolygonTest, IsPointInsideException) {
    Points pol_points;
    const Polygon pol{pol_points};

    const Point point{-1, 1};

    ASSERT_THROW(pol.is_point_inside(point), Polygon::NotEnoughPointsException);
}

TEST(PolygonTest, IsClockWiseTrue) {
    Points pol_points;
    pol_points.push_back(Point{});
    pol_points.push_back(Point{2, 0});
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{0, 2});
    const Polygon pol{pol_points};

    ASSERT_TRUE(pol.is_clockwise());
}

TEST(PolygonTest, IsClockWiseFalse) {
    Points pol_points;
    pol_points.push_back(Point{0, 2});
    pol_points.push_back(Point{2, 2});
    pol_points.push_back(Point{2, 0});
    pol_points.push_back(Point{});
    const Polygon pol{pol_points};

    ASSERT_FALSE(pol.is_clockwise());
}

TEST(PolygonTest, IsClockWiseException) {
    Points pol_points;
    const Polygon pol{pol_points};

    ASSERT_THROW(pol.is_clockwise(), Polygon::NotEnoughPointsException);
}