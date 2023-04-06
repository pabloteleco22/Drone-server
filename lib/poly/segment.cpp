#include "line.hpp"

#include <cmath>

Segment::Segment() {
    l = Line{};
}

Segment::Segment(const Segment &s) {
    l = s.l;
}

Segment::Segment(const Line &l) {
    this->l = l;
}

Segment::Segment(const Point &start, const Point &end) {
    l = Line{start, end};
}

/**
 * @brief It returns the start point of the segment
*/
Point Segment::get_start() const {
    return start;
}

/**
 * @brief It returns the end point of the segment
*/
Point Segment::get_end() const {
    return end;
}

/**
 * @brief It returns the length of the segment
*/
double Segment::length() const {
    return start.distance(end);
}

/**
 * @brief It returns the square of the length of the segment
*/
double Segment::square_length() const {
    return start.square_distance(end);
}

/**
 * @brief It returns the same segment but the start point is
 * the new end point and vice versa
*/
Segment Segment::reverse() const {
    return Segment{end, start};
}

inline bool inside(double v, double min, double max) {
    return ((min <= (v + POLY_SPLIT_EPS)) and (v <= (max + POLY_SPLIT_EPS)));
}

inline double det(double a, double b, double c, double d) {
    return (((a) * (d)) - ((b) * (c)));
}

inline double minimum(double a, double b) {
    return (((a) > (b)) ? (b) : (a));
}

inline double maximum(double a, double b) {
    return (((a) < (b)) ? (b) : (a));
}

/**
 * @brief Returns a point on the line one distance t away from
 * the start point
*/
Point Segment::get_point_along(double t) const {
    Point p{l.get_point_along(t)};
    Point min{minimum(start.x, end.x), minimum(start.y, end.y)};
    Point max{maximum(start.x, end.x), maximum(start.y, end.y)};
    
    if (!inside(p.x, min.x, max.x) or (!inside(p.y, min.y, max.y))) {
        p = get_nearest_point(p);
    }

    return p;
}

/**
 * @brief Returns the distance between the point and the line
*/
double Segment::get_distance(const Point &point) const {
    return l.get_distance(point);
}

/**
 * @brief Returns the nearest point from the segment,
 * which means that it takes into account the start
 * and end points
*/
Point Segment::get_nearest_point(const Point &point) const {
    Vector dir{l.b, -l.a};
    double u{Vector{point - start}.dot(dir) / dir.square_length()};
    if (u < 0)
        return start;
    else if (u > 1)
        return end;
    else
        return start + dir * u;
}

/**
 * @brief Returns if the point is over, under or in the line
*/
PointSide Segment::point_side(const Point &point) const {
    double s{l.a * (point.x - start.x) + l.b * (point.y - start.y)};
    if (s > 0) {
        return PointSide::Above;
    } else if (s < 0) {
        return PointSide::Below;
    } else {
        return PointSide::Inside;
    }
}

/**
 * @brief Returns whether the line and the segment intersect and
 * the point of intersection between them
 * 
 * @param 
 * line: The segment, not the line.
 *
 * @param
 * result: The intersection point.
 * 
 * @return If the line and the segment intersect
*/
bool Segment::cross_line(const Line &line, Point &result) const {
    double d{det(line.a, line.b, l.a, l.b)};
    if (d == 0)
        return false;

    result.x = -det(line.c, line.b, l.c, l.b) / d;
    result.y = -det(line.a, line.c, l.a, l.c) / d;

    return inside(result.x, minimum(start.x, end.x), maximum(start.x, end.x)) &&
            inside(result.y, minimum(start.y, end.y), maximum(start.y, end.y));
}

/**
 * @brief Returns whether the thw segments intersect and
 * the point of intersection between them
 * 
 * @param
 * result: The intersection point.
 * 
 * @return If the line and the segment intersect
*/
bool Segment::cross_line(const Segment &seg, Point &result) const {
    double d{det(l.a, l.b, seg.l.a, seg.l.b)};
    if (d == 0)
        return false;

    result.x = -det(l.c, l.b, seg.l.c, seg.l.b) / d;
    result.y = -det(l.a, l.c, seg.l.a, seg.l.c) / d;

    return inside(result.x, minimum(start.x, end.x), maximum(start.x, end.x)) &&
           inside(result.y, minimum(start.y, end.y), maximum(start.y, end.y)) &&
           inside(result.x, minimum(seg.start.x, seg.end.x), maximum(seg.start.x, seg.end.x)) &&
           inside(result.y, minimum(seg.start.y, seg.end.y), maximum(seg.start.y, seg.end.y));
}

bool cross_line(const Line &lin, const Segment &seg, Point &result) {
    return seg.cross_line(lin, result);
}

bool Segment::operator==(const Segment &other) const {
    return start == other.start and end == other.end;
}

Segment &Segment::operator=(const Segment &other) {
    l = other.l;

    return *this;
}

/**
 * @brief Returns true if l1 and l2 are the same line
*/
bool Segment::is_same(const Segment &l1, const Segment &l2) {
    return l1 == l2;
}

/**
 * @brief Returns the bisector between the two lines
*/
Line Segment::get_bisector(const Segment &seg1, const Segment &seg2) {
    if (seg1 == seg2) {
        return seg1.make_line();
    } else {
        double q1{sqrt(seg1.l.a * seg1.l.a + seg1.l.b * seg1.l.b)};
        double q2{sqrt(seg2.l.a * seg2.l.a + seg2.l.b * seg2.l.b)};

        double a{seg1.l.a / q1 - seg2.l.a / q2};
        double b{seg1.l.b / q1 - seg2.l.b / q2};
        double c{seg1.l.c / q1 - seg2.l.c / q2};

        return Line{a, b, c};
    }
}

/**
 * @brief Returns the tangent of the angle between the two lines in radians
*/
double Segment::get_tan_angle(const Segment &s1, const Segment &s2) {
    return (s1.l.a * s2.l.b - s2.l.a * s1.l.b) / (s1.l.a * s2.l.a + s1.l.b * s2.l.b);
}

std::ostream &operator<<(std::ostream &out, const Segment &s) {
    out << s.l;
    return out;
}

Line Segment::make_line() const {
    return Line{l.a, l.b, l.c};
}
