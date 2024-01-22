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

#include "point.hpp"

struct Vector {
    Point p;
    double &x = p.x;
    double &y = p.y;

    Vector(const Point &p);
    Vector(const Vector &v);
    Vector(double x = 0.0f, double y = 0.0f);
    Vector &operator=(const Vector &v);
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
    Vector unit(void) const;
    Vector norm(void) const;
    bool operator==(const Vector &v) const;
    bool operator!=(const Vector &v) const;
    Vector abs() const;
    friend Point operator+(const Point &p, const Vector &v);
    friend std::ostream &operator<<(std::ostream &out, const Vector &v);
};

using Vectors = std::vector<Vector>;
using VecIter = std::vector<Vector>::iterator;
using CVectIter = std::vector<Vector>::const_iterator;