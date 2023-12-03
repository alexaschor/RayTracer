#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "SETTINGS.hpp"
#include "shape.hpp"

// A triangle, with three points in order
class Triangle: public Shape {
private:
    Point a;
    Point b;
    Point c;
public:
    Triangle(const Point& a, const Point& b, const Point& c, Material *material): a(a), b(b), c(c) {
        this->material = material;
        origin = (a + b + c) / 3;
    };
    ~Triangle() {};
    bool intersect(Intersection &intersection) const;
    using Shape::rotate;
    void translate(const Vector& t);
    void rotate(const Vector& axis, const Real angle);
    AABB getBoundingBox() const;
};

#endif
