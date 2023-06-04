#include "vector.hpp"
#include <cmath>

Vector::Vector(const Point &p) {
    this->p = p;
};

Vector::Vector(const Vector &v) {
    p = v.p;
}

Vector::Vector(double x, double y) {
    p.x = x;
    p.y = y;
};

Vector &Vector::operator=(const Vector &v) {
    p = v.p;

    return *this;
}

Vector Vector::operator-() const {
    return Vector{-p};
}

Vector &Vector::operator+=(const Vector &v) {
    p += v.p;

    return *this;
}

Vector &Vector::operator-=(const Vector &v) {
    p -= v.p;

    return *this;
}

Vector &Vector::operator*=(double v) {
    p *= v;

    return *this;
}

Vector &Vector::operator/=(double v) {
    p /= v;

    return *this;
}

Vector Vector::operator-(const Vector &v) const {
    return Vector{p - v.p};
}

Vector Vector::operator+(const Vector &v) const {
    return Vector{p + v.p};
}

Vector Vector::operator*(const double v) const {
    return Vector{p * v};
}

Vector Vector::operator/(const double v) const {
    return Vector{p / v};
}

double Vector::dot(const Vector &v) const {
    return x * v.x + y * v.y;
}

double Vector::length(void) const {
    return sqrt(square_length());
}

double Vector::square_length(void) const {
    return p.square_distance({0, 0});
}

Vector Vector::norm(void) const {
    double l = length();
    if (l == 0)
        return Vector{};
    else
        return Vector{p / l};
}

bool Vector::operator==(const Vector &v) const {
    return p == v.p;
}

bool Vector::operator!=(const Vector &v) const {
    return p != v.p;
}

Vector Vector::abs() const {
    return Vector{p.abs()};
}

Point operator+(const Point &p, const Vector &v) {
    return Point{p.x + v.x, p.y + v.y};
}

std::ostream& operator<<(std::ostream &out, const Vector &v) {
    out << v.p;
    return out;
}