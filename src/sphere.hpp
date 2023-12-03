#ifndef SPHERE_H
#define SPHERE_H
#include "shape.hpp"
#include "SETTINGS.hpp"

// A sphere, with a origin and radius
class Sphere: public Shape {
private:
    Vector up;
    Vector meridian;
    Vector east;
    Real radius;
public:
    Sphere(const Point& origin, const Vector& up, const Vector& meridian, Real radius, Material *material)
        : up(up), meridian(meridian), radius(radius)
    {
        this->origin = origin;
        this->material = material;
        this->east = up.cross(meridian);
        this->meridian = east.cross(up);
    }
    Sphere(const Point& origin, Real radius, Material *material): up(Vector(0,1,0)), radius(radius) {
        this->origin = origin;
        this->material = material;
    }
    ~Sphere() {};
    bool intersect(Intersection &intersection) const;
    using Shape::rotate;
    void translate(const Vector& t);
    void rotate(const Vector& axis, const Real angle);
    AABB getBoundingBox() const;
};

#endif
