#ifndef CIRCLE_H
#define CIRCLE_H

#include "intersection.hpp"
#include "shape.hpp"

class Circle : public Shape {
private:
    Point origin;
    Vector normal;
    Real radius;
    Vector u, v;
public:
    Circle(const Point& origin, const Vector& normal, Real radius, Material* material);
    Circle(const Point& origin, const Vector& normal, Real radius, const Vector& u, Material* material);
    ~Circle() {};

    bool intersect(Intersection &intersection) const;
    using Shape::rotate;
    void translate(const Vector& t);
    void rotate(const Vector& axis, const Real angle);
    AABB getBoundingBox() const;
};

#endif
