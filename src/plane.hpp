#ifndef PLANE_H
#define PLANE_H

#include "shape.hpp"

class Plane: public Shape {
private:
    Vector normal;
    Vector u, v;
    Real width, height;
public:
    Plane(const Point& point, const Vector& normal, Real width, Real height, const Vector& u, Material *material, bool center=false);
    ~Plane() {};
    bool intersect(Intersection &intersection) const;
    using Shape::rotate;
    void translate(const Vector& t);
    void rotate(const Vector& axis, const Real angle);
    AABB getBoundingBox() const;

};


#endif
