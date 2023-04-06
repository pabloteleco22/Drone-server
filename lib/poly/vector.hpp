#pragma once

#include "point.hpp"

struct Vector {
    Point p;
    double &x = p.x;
    double &y = p.y;

    Vector(const Point &p);
    Vector(const Vector &v);
    Vector(double x=0.0f, double y=0.0f);
    void operator=(const Vector &v);
    Vector operator-() const;
    Vector &operator+=(const Vector &v);
    Vector &operator-=(const Vector &v);
    Vector &operator*=(double v);
    Vector &operator/=(double v);
    Vector operator-(const Vector &v) const;
    Vector operator+(const Vector &v) const;
    Vector operator*(const double v) const;
    Vector operator/(const double v) const;
    double dot(const Vector &v) const;
    double length(void) const;
    double square_length(void) const;
    Vector norm(void) const;
    bool operator==(const Vector &v) const;
    bool operator!=(const Vector &v) const;
    Vector abs() const;
    friend Point operator+(const Point &p, const Vector &v);
    friend std::ostream& operator<<(std::ostream &out, const Vector &v);
};

using Vectors = std::vector<Vector>;
using VecIter = std::vector<Vector>::iterator;
using CVectIter = std::vector<Vector>::const_iterator;