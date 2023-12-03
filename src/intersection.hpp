#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "SETTINGS.hpp"
#include "ray.hpp"
#include "material.hpp"

// Forward declare Shape class
class Shape;
class Scene;

class Intersection {
public:
    const Shape *shape;
    Ray ray;
    Real t;

    Real u, v;

    Vector normal;
    Vector tangent;
    Vector bitangent;

    bool intersected;
    bool DEBUG;

    int bouncesLeft;


    Intersection(const Ray& ray);

    Point getPosition() const;

    Color getColor(const Scene *scene) const;

    void print();

};

#endif
