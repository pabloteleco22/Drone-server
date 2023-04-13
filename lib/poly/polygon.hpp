#pragma once

#include "line.hpp"
#include <string>
#include <exception>

class Polygon {
private:
    Points vertex;

    static bool get_cut(const Segment &s1, const Segment &s2, double s,
                const Polygon &poly1, const Polygon &poly2,
                Segment &cut);

public:
    Polygon();
    Polygon(const Points &p);
    Polygon(const Polygon &p);

    class NotEnoughPointsException : public std::exception {
        std::string message{"The polygon has not enough vertices"};
        public:
            NotEnoughPointsException();
            NotEnoughPointsException(const std::string &message);
            NotEnoughPointsException(const char *message);
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
    bool split(double square, Polygon &remainder, Polygon &interest, Segment &cut_line) const;

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
    Point count_center(void) const;

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
     * excludeLine1: The index of the first segment to be disregarded in the analysis.
     * @param
     * excludeLine2: The index of the second segment to be disregarded in the analysis.
    */
    bool is_segment_inside(const Segment &segment, size_t excludeLine1, size_t excludeLine2) const;
    
    /**
     * @brief Returns true if the vertex are in clock wise order.
    */
    bool is_clockwise(void) const;

    const Points get_vectors(void) const {
        return vertex;
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
        return vertex.empty();
    }

    Polygon &operator=(const Polygon &p) {
        vertex = p.vertex;
        return *this;
    }

    Point &operator[](size_t index) {
        return vertex[index];
    }

    Point operator[](size_t index) const {
        return vertex[index];
    }

    /**
     * @brief Removes all the vertex of the polygon.
    */
    void clear(void) {
        vertex.clear();
    }

    /**
     * @brief Returns the number of vertex of the polygon.
    */
    size_t size(void) const {
        return vertex.size();
    }
};

struct Polygons {
    Polygons(const Segment &l1, const Segment &l2);
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