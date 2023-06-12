#include "polygon.hpp"

#include <cfloat>
#include <algorithm>
#include <exception>
#include <cmath>

Polygons::Polygons(const Segment &s1, const Segment &s2) {
    bisector = Segment::get_bisector(s1, s2);

    Point p1{s1.get_start()};
    Point p2{s1.get_end()};
    Point p3{s2.get_start()};
    Point p4{s2.get_end()};

    p1_exist = false;
    p4_exist = false;
    if (p1 != p4) {
        Line l1s{p1, bisector.get_nearest_point(p1)};
        Point np1;
        p1_exist = s2.cross_line(l1s, np1) && np1 != p4;
        if (p1_exist) {
            left_triangle.push_back(p1);
            left_triangle.push_back(p4);
            left_triangle.push_back(np1);

            trapezoid.push_back(np1);
        } else {
            trapezoid.push_back(p4);
        }

        Line l2e{p4, bisector.get_nearest_point(p4)};
        Point np4;
        p4_exist = s1.cross_line(l2e, np4) && np4 != p1;
        if (p4_exist) {
            left_triangle.push_back(p4);
            left_triangle.push_back(p1);
            left_triangle.push_back(np4);

            trapezoid.push_back(np4);
        } else {
            trapezoid.push_back(p1);
        }
    } else {
        trapezoid.push_back(p4);
        trapezoid.push_back(p1);
    }

    p2_exist = false;
    p3_exist = false;
    if (p2 != p3) {
        Line l2s{p3, bisector.get_nearest_point(p3)};
        Point np3;
        p3_exist = s1.cross_line(l2s, np3) && np3 != p2;
        if (p3_exist) {
            right_triangle.push_back(p3);
            right_triangle.push_back(p2);
            right_triangle.push_back(np3);

            trapezoid.push_back(np3);
        } else {
            trapezoid.push_back(p2);
        }

        Line l1e{p2, bisector.get_nearest_point(p2)};
        Point np2;
        p2_exist = s2.cross_line(l1e, np2) && np2 != p3;
        if (p2_exist) {
            right_triangle.push_back(p2);
            right_triangle.push_back(p3);
            right_triangle.push_back(np2);

            trapezoid.push_back(np2);
        } else {
            trapezoid.push_back(p3);
        }
    } else {
        trapezoid.push_back(p2);
        trapezoid.push_back(p3);
    }

    left_triangle_square = left_triangle.count_square();
    trapezoid_square = trapezoid.count_square();
    right_triangle_square = right_triangle.count_square();

    total_square = left_triangle_square + trapezoid_square + right_triangle_square;
}

bool Polygons::find_cut_line(double square, Segment &cut_line) {
    if (square > total_square) {
        return false;
    }

    if (!left_triangle.empty() && square < left_triangle_square) {
        double m{square / left_triangle_square};
        Point p{left_triangle[1] + (left_triangle[2] - left_triangle[1]) * m};
        if (p1_exist) {
            cut_line = Segment{p, left_triangle[0]};
            return true;
        } else if(p4_exist) {
            cut_line = Segment{left_triangle[0], p};
            return true;
        }
    } else if(left_triangle_square < square && square < (left_triangle_square + trapezoid_square)) {
        Segment t{trapezoid[0], trapezoid[3]};
        double tgA{Segment::get_tan_angle(t, bisector)};
        double S{square - left_triangle_square};
        double m;
        if (fabs(tgA) > POLY_SPLIT_EPS) {
            double a{Segment(trapezoid[0], trapezoid[1]).length()};
            double b{Segment(trapezoid[2], trapezoid[3]).length()};
            double hh{2.0 * trapezoid_square / (a + b)};
            double d{a * a - 4.0 * tgA * S};
            double h{-(-a + sqrt(d)) / (2.0 * tgA)};
            m = h / hh;
        } else {
            m = S / trapezoid_square;
        }
        Point p{trapezoid[0] + (trapezoid[3] - trapezoid[0]) * m};
        Point pp{trapezoid[1] + (trapezoid[2] - trapezoid[1]) * m};

        cut_line = Segment{p, pp};

        return true;
    } else if(!right_triangle.empty() && square > left_triangle_square + trapezoid_square) {
        double S{square - left_triangle_square - trapezoid_square};
        double m{S / right_triangle_square};
        Point p{right_triangle[2] + (right_triangle[1] - right_triangle[2]) * m};
        if (p3_exist) {
            cut_line = Segment{right_triangle[0], p};
            return true;
        } else if (p2_exist) {
            cut_line = Segment{p, right_triangle[0]};
            return true;
        }
    }

    return false;
}

Polygon::NotEnoughPointsException::NotEnoughPointsException() {}

Polygon::NotEnoughPointsException::NotEnoughPointsException(const std::string &message) {
    this->message = std::string{message};
}

Polygon::NotEnoughPointsException::NotEnoughPointsException(const char *message) {
    this->message = std::string{message};
}

const char *Polygon::NotEnoughPointsException::what() const noexcept {
    return message.c_str();
}

Polygon::CannotSplitException::CannotSplitException () {}

Polygon::CannotSplitException::CannotSplitException (const std::string &message) {
    this->message = std::string{message};
}

Polygon::CannotSplitException::CannotSplitException (const char *message) {
    this->message = std::string{message};
}

const char *Polygon::CannotSplitException::what() const noexcept {
    return message.c_str();
}

Polygon::Polygon() {}

Polygon::Polygon(const Polygon &p) {
    vertices = p.vertices;
}

Polygon::Polygon(const Points &p) {
    vertices = p;
}

double Polygon::count_square_signed(void) const {
    size_t pointsCount{vertices.size()};
    if (pointsCount < 3) {
        return 0;
    }

    double result{0};
    for (size_t i = 0; i < pointsCount; i++) {
        if (i == 0)
            result += vertices[i].x * (vertices[pointsCount - 1].y - vertices[i + 1].y);
        else if (i == pointsCount - 1)
            result += vertices[i].x * (vertices[i - 1].y - vertices[0].y);
        else
            result += vertices[i].x * (vertices[i - 1].y - vertices[i + 1].y);
    }

    return result / 2.0;
}

double Polygon::count_square() const {
    return fabs(count_square_signed());
}

void Polygon::split(double square, Polygon &poly1, Polygon &poly2, Segment &cut_line) const {
    int polygon_size{static_cast<int>(vertices.size())};

    Points polygon{vertices};
    if (!is_clockwise()) {
        std::reverse(polygon.begin(), polygon.end());
    }

    poly1.clear();
    poly2.clear();

    if (count_square() - square <= POLY_SPLIT_EPS) {
        poly1 = *this;
        throw Polygon::CannotSplitException{"The required area is too big"};
    }

    bool min_cut_line_exists{false};
    double min_sq_length = DBL_MAX;

    for (int i = 0; i < polygon_size - 1; i++) {
        for (int j = i + 1; j < polygon_size; j++) {
            Polygon p1;
            Polygon p2;

            p1.clear();
            p2.clear();
            
            int pc1{j - i};
            for (int z = 1; z <= pc1; ++z) {
                p1.push_back(polygon[z + i]);
            }

            int pc2{polygon_size - pc1};
            for (int z = 1; z <= pc2; ++z) {
                p2.push_back(polygon[(z + j) % polygon_size]);
            }

            Line l1{polygon[i], polygon[i + 1]};
            Line l2{polygon[j], polygon[(j + 1) < polygon_size ? (j + 1) : 0]};
            Segment cut;

            if (get_cut(l1, l2, square, p1, p2, cut)) {
                double sq_length{cut.square_length()};

                if (sq_length < min_sq_length && is_segment_inside(cut, i, j)) {
                    min_sq_length = sq_length;
                    poly1 = p1;
                    poly2 = p2;
                    cut_line = cut;
                    min_cut_line_exists = true;
                }
            }
        }
    }

    if (min_cut_line_exists) {
        poly1.push_back(cut_line.get_start());
        poly1.push_back(cut_line.get_end());

        poly2.push_back(cut_line.get_end());
        poly2.push_back(cut_line.get_start());
    } else {
        poly1 = Polygon{polygon};
        throw Polygon::CannotSplitException{"The cut line does not exists"};
    }
}

double Polygon::find_distance(const Point &point) const {
    double distance{std::numeric_limits<double>::infinity()};
    int poly_size{static_cast<int>(vertices.size())};
    if (poly_size < 2)
        throw Polygon::NotEnoughPointsException{"The polygon has not enough vertices"};

    for (int i = 0; i < poly_size - 1; i++) {
        Segment seg{vertices[i], vertices[i + 1]};
        Point p{seg.get_nearest_point(point)};
        double l{p.distance(point)};
        if (l < distance)
            distance = l;
    }
    
    Segment seg{vertices[poly_size - 1], vertices[0]};
    Point p{seg.get_nearest_point(point)};
    double l{p.distance(point)};
    if (l < distance)
        distance = l;

    return distance;
}

Point Polygon::find_nearest_point(const Point &point) const {
    Point result;
    double distance{std::numeric_limits<double>::infinity()};
    int poly_size{static_cast<int>(vertices.size())};
    if (poly_size < 2)
        throw Polygon::NotEnoughPointsException{"The polygon has not enough vertices"};

    for (int i = 0; i < poly_size - 1; i++) {
        Segment seg{vertices[i], vertices[i + 1]};
        Point p{seg.get_nearest_point(point)};
        double l{(p.distance(point))};
        if (l < distance) {
            distance = l;
            result = p;
        }
    }

    Segment seg{vertices[poly_size - 1], vertices[0]};
    Point p{seg.get_nearest_point(point)};
    double l{(p.distance(point))};
    if (l < distance) {
        distance = l;
        result = p;
    }

    return result;
}

Point Polygon::find_center() const {
    int n{static_cast<int>(vertices.size())};
    if (n <= 0)
        throw Polygon::NotEnoughPointsException{"The polygon has zero vertices"};

    Point result;
    for (Point v : vertices) {
        result += v;
    }
    result /= n;

    return result;
}

void Polygon::split_nearest_edge(const Point &point) {
    Point result;
    int ri{-1};
    double distance{std::numeric_limits<double>::infinity()};
    int poly_size{static_cast<int>(vertices.size())};
    if (poly_size < 2)
        throw Polygon::NotEnoughPointsException{"The polygon has less than two vertices"};

    for (int i = 0; i < poly_size - 1; i++) {
        Segment seg{vertices[i], vertices[i + 1]};
        Point p{seg.get_nearest_point(point)};
        double l{p.distance(point)};
        if (l < distance) {
            distance = l;
            ri = i;
            result = p;
        }
    }
    Segment seg{vertices[poly_size - 1], vertices[0]};
    Point p{seg.get_nearest_point(point)};
    double l{p.distance(point)};
    if (l < distance) {
        distance = l;
        ri = vertices.size() - 1;
        result = p;
    }

    if ((ri != -1) and (vertices[ri] != result) and (vertices[ri + 1] != result)) {
        vertices.insert(vertices.begin() + ri + 1, result);
    }
}

bool Polygon::is_point_inside(const Point &point) const {
    int pointsCount{static_cast<int>(vertices.size()) - 1};
    if (pointsCount < 2)
        throw Polygon::NotEnoughPointsException{"The polygon has not enough vertices"};

    Segment s{Line{point, Vector{0.0, 1e100}}};
    int result{0};
    Point p;
    for (int i = 0; i < pointsCount; i++) {
        Segment seg{vertices[i], vertices[i + 1]};
        result += s.cross_line(seg, p);
    }
    Segment seg{vertices[pointsCount], vertices[0]};
    result += s.cross_line(seg, p);
    return result % 2 != 0;
}

bool Polygon::is_segment_inside(const Segment &segment, size_t excludeLine1, size_t excludeLine2) const {
    size_t pointsCount{vertices.size()};

    if (pointsCount < 3)
        throw Polygon::NotEnoughPointsException{"The polygon has not enough vertices"};

    for (size_t i = 0; i < pointsCount; i++) {
        if (i != excludeLine1 && i != excludeLine2) {
            Point p1{vertices[i]};
            Point p2{vertices[i + 1 < pointsCount ? i + 1 : 0]};
            Point p;
            if ((Segment{p1, p2}.cross_line(segment, p)) and
                (p1.square_distance(p) > POLY_SPLIT_EPS) and
                (p2.square_distance(p) > POLY_SPLIT_EPS)) {
                return false;
            }
        }
    }

    return is_point_inside(segment.get_point_along(0.5));
}

bool Polygon::is_clockwise() const {
    if (vertices.size() < 2)
        throw Polygon::NotEnoughPointsException{"The polygon has not enough vertices"};

    double sum{0};
    int t{static_cast<int>(vertices.size()) - 1};
    for (int i = 0; i < t; i++) {
        sum += (vertices[i + 1].x - vertices[i].x) * (vertices[i + 1].y + vertices[i].y);
    }
    sum += (vertices[0].x - vertices[t].x) * (vertices[0].y + vertices[t].y);
    return sum <= 0;
}

bool Polygon::get_cut(const Segment &s1, const Segment &s2, double s,
            const Polygon &poly1, const Polygon &poly2,
            Segment &cut) {
    double sn1{s + poly2.count_square_signed()};
    double sn2{s + poly1.count_square_signed()};

    bool success{false};

    if (sn1 > 0) {
        Polygons res{s1, s2};

        if (res.find_cut_line(sn1, cut)) {
            success = true;
        }
    } else if (sn2 > 0) {
        Polygons res{s2, s1};

        if (res.find_cut_line(sn2, cut)) {
            cut = cut.reverse();
            success = true;
        }
    }

    return success;
}

void Polygon::push_back(const Point &p) {
    vertices.push_back(p);
}
