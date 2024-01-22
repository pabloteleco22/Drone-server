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

#include "line.hpp"
#include <string>
#include <exception>

class Polygon {
private:
    Points vertices;

    static bool get_cut(const Segment &s1, const Segment &s2, double s, const Polygon &poly1, const Polygon &poly2, Segment &cut);

public:
    Polygon();
    Polygon(const Polygon &p);

    Polygon(const Points &p);

    class NotEnoughPointsException : public std::exception {
        std::string message{ "The polygon has not enough vertices" };
    public:
        NotEnoughPointsException();
        NotEnoughPointsException(const std::string &message);
        NotEnoughPointsException(const char *message);
        const char *what() const noexcept override;
    };

    class CannotSplitException : public std::exception {
        std::string message{ "The polygon has not enough vertices" };
    public:
        CannotSplitException();
        CannotSplitException(const std::string &message);
        CannotSplitException(const char *message);
        const char *what() const noexcept override;
    };

    /**
     * @brief Returns the polygon area.
    */
    double count_square(void) const;
    double count_square_signed(void) const;

    /**
     * @brief Split the polygon into two parts with the specified area.
     *
     * @param
     * square: The area of the result poly2.
     * @param
     * poly1: The resulting polygon containing the area that has
     * not been left poly2.
     * @param
     * poly2: The resulting polygon with the specified area.
     * @param
     * cut_line: The line dividing the two polygons.
     *
     * @returns
     * true: if it is possible.
     * false: if it is not possible.
    */
    void split(double square, Polygon &poly1, Polygon &poly2, Segment &cut_line) const;

    /**
     * @brief Returns the distance between the nearest point of the polygon
     * and the point passed by parameters.
     *
     * @throws
     * Polygon::VoidPolygonExcception: if the polygon contains no points.
    */
    double find_distance(const Point &point) const;

    /**
     * @brief Returns the point of the polygon nearest to the one passed by
     * parameters.
     *
     * @throws
     * Polygon::VoidPolygonExcception: if the polygon contains no points.
    */
    Point find_nearest_point(const Point &point) const;

    /**
     * @brief Returns the centroid of the polygon.
     *
     * @throws
     * Polygon::VoidPolygonExcception: if the polygon contains no points.
    */
    Point find_center(void) const;

    /**
     * @brief Generates a new vertex in the polygon at the nearest point
     * between the passed by parameter and the edge of the polygon.
    */
    void split_nearest_edge(const Point &point);

    /**
     * @brief Returns true if the point passed by parameters is contained
     * within the edges of the polygon.
    */
    bool is_point_inside(const Point &point) const;

    /**
     * @brief Returns true if the segment passed by parameters is contained
     * within the edges of the polygon.
     *
     * @param
     * exclude_line1: The index of the first segment to be disregarded in the analysis.
     * @param
     * exclude_line2: The index of the second segment to be disregarded in the analysis.
    */
    bool is_segment_inside(const Segment &segment, size_t exclude_line1, size_t exclude_line2) const;

    /**
     * @brief Returns true if the vertices are in clock wise order.
    */
    bool is_clockwise(void) const;

    const Points get_vertices(void) const {
        return vertices;
    }

    /**
     * @brief If the point passed by parameters was not a vertex of the
     * polygon, now it is.
    */
    void push_back(const Point &point);

    /**
     * @brief Returns true if the polygon has no vertex.
    */
    bool empty(void) const {
        return vertices.empty();
    }

    Polygon &operator=(const Polygon &p) {
        vertices = p.vertices;
        return *this;
    }

    Point &operator[](size_t index) {
        return vertices[index];
    }

    Point operator[](size_t index) const {
        return vertices[index];
    }

    /**
     * @brief Removes all the vertex of the polygon.
    */
    void clear(void) {
        vertices.clear();
    }

    /**
     * @brief Returns the number of vertex of the polygon.
    */
    size_t size(void) const {
        return vertices.size();
    }
};

namespace poly_private {
    struct Polygons {
        Polygons(const Segment &s1, const Segment &s2);
        bool find_cut_line(double square, Segment &cut_line);

        Line bisector;

        Polygon left_triangle;
        Polygon trapezoid;
        Polygon right_triangle;

        bool p1_exist;
        bool p2_exist;
        bool p3_exist;
        bool p4_exist;

        double left_triangle_square;
        double trapezoid_square;
        double right_triangle_square;
        double total_square;
    };
};