#ifndef CYLINDER_H
#define CYLINDER_H

#include "SETTINGS.hpp"
#include "shape.hpp"
#include "tube.hpp"
#include "circle.hpp"

using namespace std;

class Cylinder: public ShapeGroup {
private:
    Tube* tube;
    Circle* startCap;
    Circle* endCap;

public:
    Cylinder(const Point& origin, const Vector& axis, Real radius, Real length, const Vector& u, Material *material) {
        Vector axn(axis.normalized());

        tube = new Tube(origin, axn, radius, length, u, material);

        startCap = new Circle(origin, -axn, radius, u, material);
        endCap = new Circle(origin + (axn * length), axn, radius, u, material);

        addShape(tube);
        addShape(endCap);
        addShape(startCap);
    }

    Cylinder(const Point& origin, const Point& end, Real radius, const Vector& u, Material *material) {
        Vector axn = (end - origin).normalized();
        Real length = (end - origin).dot(axn);

        tube = new Tube(origin, end, radius, u, material);
        startCap = new Circle(origin, -axn, radius, u, material);
        endCap = new Circle(origin + (axn * length), axn, radius, u, material);

        addShape(tube);
        addShape(startCap);
        addShape(endCap);
    }

    ~Cylinder() {
        delete tube;
        delete startCap;
        delete endCap;
    }

};


#endif
