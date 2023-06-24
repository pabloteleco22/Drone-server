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

#pragma once

#include <ostream>
#include <vector>
#include <limits>

const double POLY_SPLIT_EPS = 1E-6;

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
    operator std::string() const;
    double distance(const Point &p) const;
    double square_distance(const Point &p) const;
    Point abs() const;
    friend std::ostream& operator<<(std::ostream &out, const Point &v);
};

using Points = std::vector<Point>;
using PointIter = std::vector<Point>::iterator;
using CPointIter = std::vector<Point>::const_iterator;