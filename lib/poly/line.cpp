#include "line.hpp"

#include <cassert>
#include <cmath>

Line::Line() {
    a = 0;
    b = 0;
    c = 0;
    p1 = Point{};
    p2 = Point{};
}

Line::Line(const Line &other) {
    a = other.a;
    b = other.b;
    c = other.c;
    p1 = other.p1;
    p2 = other.p2;
}

Line::Line(const Point &p1, const Point &p2) : p1(p1), p2(p2) {
    a = p1.y - p2.y;
    b = p2.x - p1.x;
    c = p1.x * p2.y - p2.x * p1.y;
}

Line::Line(double a, double b, double c) : a{a}, b{b}, c{c} {
    if (fabs(a) <= POLY_SPLIT_EPS && fabs(b) >= POLY_SPLIT_EPS) {
        p1.x = -1000;
        p1.y = -(c / b);

        p2.x = 1000;
        p2.y = p1.y;
    } else if (fabs(b) <= POLY_SPLIT_EPS && fabs(a) >= POLY_SPLIT_EPS) {
        p1.x = -(c / a);
        p1.y = -1000;

        p2.x = p1.x;
        p2.y = 1000;
    } else {
        p1.x = -1000;
        p1.y = -((a * p1.x + c) / b);

        p2.x = 1000;
        p2.y = -((a * p2.x + c) / b);
    }
}

Line::Line(Point p, Vector v) : Line(p, p + v) {}

double Line::get_distance(const Point &point) const {
    double n{a * point.x + b * point.y + c};
    double m{sqrt(a * a + b * b)};
    assert(m != 0);
    return n / m;
}

Point Line::get_nearest_point(const Point &point) const {
    Vector dir{b, -a};
    double u{Vector{point - p1}.dot(dir) / dir.square_length()};
    return p1 + dir * u;
}

PointSide Line::point_side(const Point &point) const {
    double s{a * (point.x - p1.x) + b * (point.y - p1.y)};
    if (s > 0) {
        return PointSide::Above;
    } else if (s < 0) {
        return PointSide::Below;
    } else {
        return PointSide::Inside;
    }
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

bool Line::cross_line(const Line &line, Point &result) const {
    double d{det(a, b, line.a, line.b)};
    if (d == 0)
        return false;

    result.x = -det(c, b, line.c, line.b) / d;
    result.y = -det(a, c, line.a, line.c) / d;

    return true;
}

bool Line::operator==(const Line &other) const {
    return (point_side(other.p1) == PointSide::Inside) and (point_side(other.p2) == PointSide::Inside);
}

Line &Line::operator=(const Line &other) {
    p1 = other.p1;
    p2 = other.p2;
    a = other.a;
    b = other.b;
    c = other.c;

    return *this;
}

Line Line::get_bisector(const Line &l1, const Line &l2) {
    if (l1 == l2) {
        return Line{l1};
    } else {
        double q1{sqrt(l1.a * l1.a + l1.b * l1.b)};
        double q2{sqrt(l2.a * l2.a + l2.b * l2.b)};

        double a{l1.a / q1 - l2.a / q2};
        double b{l1.b / q1 - l2.b / q2};
        double c{l1.c / q1 - l2.c / q2};

        return Line{a, b, c};
    }
}

double Line::get_tan_angle(const Line &l1, const Line &l2) {
    return (l1.a * l2.b - l2.a * l1.b) / (l1.a * l2.a + l1.b * l2.b);
}

Point Line::get_p1() const {
    return p1;
}

Point Line::get_p2() const {
    return p2;
}

double Line::square_length() const {
    double x{p2.x - p1.x};
    double y{p2.y - p1.y};

    return x * x + y * y;
}

Point Line::get_point_along(double t) const {
    return p1 + Vector{p2 - p1}.norm() * t;
}

std::ostream &operator<<(std::ostream &out, const Line &l) {
    out << "[" << l.a << ", " << l.b << ", " << l.c << "]-{" << l.p1 << ", " << l.p2 << "}";
    return out;
}