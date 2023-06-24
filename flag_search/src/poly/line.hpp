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

#include "vector.hpp"

enum class PointSide {
    Above,
    Inside,
    Below
};

class Line {
    private:
        double a{0}; // -Increase of y
        double b{0}; // Increase of x
        double c{0}; // Determinant of the segment
        Point p1, p2;

    public:
        Line();
        Line(const Line &other);
        Line(const Point &p1, const Point &p2);
        Line(double a, double b, double c);
        Line(Point p, Vector v);

        /**
         * @brief It returns the start point of the segment
        */
        Point get_p1() const;

        /**
         * @brief It returns the end point of the segment
        */
        Point get_p2() const;

        /**
         * @brief It returns the square of the length of the segment
        */
        double square_length() const;

        /**
         * @brief Returns a point on the line one distance t away from
         * the start point
        */
        Point get_point_along(double t) const;

        /**
         * @brief Returns the distance between the point and the line
        */
        double get_distance(const Point &point) const;

        /**
         * @brief Returns the nearest point from the line
        */
        Point get_nearest_point(const Point &point) const;

        /**
         * @brief Returns if the point is over, under or in the line
        */
        PointSide point_side(const Point &point) const;

        /**
         * @brief Returns whether the two lines intersect and
         * the point of intersection between them
         * 
         * @param
         * result: The intersection point.
         * 
         * @return If the line and the segment intersect
        */
        bool cross_line(const Line &line, Point &result) const;

        bool operator==(const Line &other) const;

        Line &operator=(const Line &other);

        /**
         * @brief Returns the bisector between the two lines
        */
        static Line get_bisector(const Line &l1, const Line &l2);

        /**
         * @brief Returns the tangent of the angle between the two lines in radians
        */
        static double get_tan_angle(const Line &l1, const Line &l2);

        friend std::ostream &operator<<(std::ostream &out, const Line &l);
        friend class Segment;
};

class Segment {
    private:
        Line l;
        Point &start = l.p1;
        Point &end = l.p2;

    public:
        Segment();
        Segment(const Segment &s);
        Segment(const Line &l);
        Segment(const Point &start, const Point &end);
        Line make_line() const;

        /**
         * @brief It returns the start point of the segment
        */
        Point get_start() const;

        /**
         * @brief It returns the end point of the segment
        */
        Point get_end() const;

        /**
         * @brief It returns the length of the segment
        */
        double length() const;

        /**
         * @brief It returns the square of the length of the segment
        */
        double square_length() const;

        /**
         * @brief It returns the same segment but the start point is
         * the new end point and vice versa
        */
        Segment reverse() const;

        /**
         * @brief Returns a point on the line one distance t away from
         * the start point
        */
        Point get_point_along(double t) const;

        /**
         * @brief Returns the distance between the point and the line
        */
        double get_distance(const Point &point) const;

        /**
         * @brief Returns the nearest point from the segment,
         * which means that it takes into account the start
         * and end points
        */
        Point get_nearest_point(const Point &point) const;

        /**
         * @brief Returns if the point is over, under or in the line
        */
        PointSide point_side(const Point &point) const;

        /**
         * @brief Returns whether the line and the segment intersect and
         * the point of intersection between them
         * 
         * @param 
         * line: The line, not the segment.
         *
         * @param
         * result: The intersection point.
         * 
         * @return If the line and the segment intersect
        */
        bool cross_line(const Line &line, Point &result) const;

        /**
         * @brief Returns whether the the segments intersect and
         * the point of intersection between them
         * 
         * @param
         * result: The intersection point.
         * 
         * @return If the line and the segment intersect
        */
        bool cross_line(const Segment &seg, Point &result) const;

        bool operator==(const Segment &other) const;
        Segment &operator=(const Segment &other);

        /**
         * @brief Returns the bisector between the two lines
        */
        static Line get_bisector(const Segment &s1, const Segment &s2);

        /**
         * @brief Returns the tangent of the angle between the two lines in radians
        */
        static double get_tan_angle(const Segment &s1, const Segment &s2);

        friend std::ostream &operator<<(std::ostream &out, const Segment &s);
};
