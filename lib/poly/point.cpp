#include "point.hpp"

#include <cmath>

Point Point::operator-() const {
    return Point{-x, -y};
}

Point &Point::operator+=(const Point &p) {
    x += p.x;
    y += p.y;

    return *this;
}

Point &Point::operator-=(const Point &p) {
    x -= p.x;
    y -= p.y;

    return *this;
}

Point &Point::operator*=(double p) {
    x *= p;
    y *= p;

    return *this;
}

Point &Point::operator/=(double p) {
    x /= p;
    y /= p;

    return *this;
}

Point Point::operator-(const Point &p) const {
    return Point{x - p.x, y - p.y};
}

Point Point::operator+(const Point &p) const {
    return Point{x + p.x, y + p.y};
}

Point Point::operator*(const double p) const {
    return Point{x * p, y * p};
}

Point Point::operator/(const double p) const {
    return Point{x / p, y / p};
}

bool Point::operator==(const Point &p) const {
    return x == p.x && y == p.y;
}

bool Point::operator!=(const Point &p) const {
    return fabs(x - p.x) >= POLY_SPLIT_EPS || fabs(y - p.y) >= POLY_SPLIT_EPS;
}

double Point::distance(const Point &p) const {
    return sqrt(square_distance(p));
}

double Point::square_distance(const Point &p) const {
    double dx = fabs(x - p.x);
    double dy = fabs(y - p.y);

    return dx * dx + dy * dy;
}

Point Point::abs() const {
    return Point{fabs(x), fabs(y)};
}

std::ostream& operator<<(std::ostream &out, const Point &v) {
    out << "(" << v.x << ", " << v.y << ")";
    return out;
}