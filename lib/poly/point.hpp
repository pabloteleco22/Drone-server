#pragma once

#include <ostream>
#include <vector>
#include <string>

const double POLY_SPLIT_EPS{1E-6};

struct Point {
    double x, y;

    Point(double x = 0.0f, double y = 0.0f) : x(x), y(y) {};

    Point operator-() const;
    Point &operator+=(const Point &p);
    Point &operator-=(const Point &p);
    Point &operator*=(double p);
    Point &operator/=(double p);
    Point operator-(const Point &p) const;
    Point operator+(const Point &p) const;
    Point operator*(const double p) const;
    Point operator/(const double p) const;
    bool operator==(const Point &p) const;
    bool operator!=(const Point &p) const;
    double distance(const Point &p) const;
    double square_distance(const Point &p) const;
    Point abs() const;
    operator std::string() const;
    friend std::ostream& operator<<(std::ostream &out, const Point &v);
};

using Points = std::vector<Point>;
using PointIter = std::vector<Point>::iterator;
using CPointIter = std::vector<Point>::const_iterator;