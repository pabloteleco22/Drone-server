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
    return fabs(x - p.x) <= POLY_SPLIT_EPS && fabs(y - p.y) <= POLY_SPLIT_EPS;
}

bool Point::operator!=(const Point &p) const {
    return fabs(x - p.x) > POLY_SPLIT_EPS || fabs(y - p.y) > POLY_SPLIT_EPS;
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

Point::operator std::string() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

std::ostream& operator<<(std::ostream &out, const Point &v) {
    out << "(" << v.x << ", " << v.y << ")";
    return out;
}