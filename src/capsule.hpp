#ifndef CAPSULE_H
#define CAPSULE_H

#include "SETTINGS.hpp"
#include "shape.hpp"
#include "tube.hpp"
#include "sphere.hpp"

class Capsule: public ShapeGroup {
private:
    Tube* tube;
    Sphere* startCap;
    Sphere* endCap;

public:
    Point& start = startCap->origin;
    Point& end = endCap->origin;

    Capsule(const Point& origin, const Vector& axis, Real radius, Real length, const Vector& u, Material *material) {
        Vector axn(axis.normalized());

        tube = new Tube(origin, axn, radius, length, u, material);

        startCap = new Sphere(origin, u, -axn, radius, material);
        endCap = new Sphere(origin + (axn * length), u, axn, radius, material);

        addShape(tube);
        addShape(endCap);
        addShape(startCap);
    }

    Capsule(const Point& origin, const Point& end, Real radius, const Vector& u, Material *material) {
        Vector axn = (end - origin).normalized();
        Real length = (end - origin).dot(axn);

        tube = new Tube(origin, end, radius, u, material);
        startCap = new Sphere(origin, u, -axn, radius, material);
        endCap = new Sphere(origin + (axn * length), u, axn, radius, material);

        addShape(tube);
        addShape(startCap);
        addShape(endCap);
    }

    ~Capsule() {
        delete tube;
        delete startCap;
        delete endCap;
    }

};
#endif
