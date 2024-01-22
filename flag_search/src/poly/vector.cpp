/**
 * The MIT License (MIT)
 * Copyright (c) 2016 Grabarchuk Viktor
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
    return Vector{ -p };
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
    return Vector{ p - v.p };
}

Vector Vector::operator+(const Vector &v) const {
    return Vector{ p + v.p };
}

Vector Vector::operator*(const double v) const {
    return Vector{ p * v };
}

Vector Vector::operator/(const double v) const {
    return Vector{ p / v };
}

double Vector::dot(const Vector &v) const {
    return x * v.x + y * v.y;
}

double Vector::length(void) const {
    return sqrt(square_length());
}

double Vector::square_length(void) const {
    return p.square_distance({ 0, 0 });
}

Vector Vector::unit(void) const {
    double l = length();
    if (l == 0)
        return Vector{};
    else
        return Vector{ p / l };
}

Vector Vector::norm(void) const {
    if (length() == 0)
        return Vector{};
    else
        return Vector{ y, -x };
}

bool Vector::operator==(const Vector &v) const {
    return p == v.p;
}

bool Vector::operator!=(const Vector &v) const {
    return p != v.p;
}

Vector Vector::abs() const {
    return Vector{ p.abs() };
}

Point operator+(const Point &p, const Vector &v) {
    return Point{ p.x + v.x, p.y + v.y };
}

std::ostream &operator<<(std::ostream &out, const Vector &v) {
    out << v.p;
    return out;
}