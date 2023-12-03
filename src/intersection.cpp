#include "intersection.hpp"
#include "shape.hpp"
#include "scene.hpp"

Intersection::Intersection(const Ray& ray):
    shape(NULL), ray(Ray(ray)), t(0), u(0), v(0), intersected(false),
    DEBUG(false), bouncesLeft(-1) {}


Color Intersection::getColor(const Scene *scene) const{
    if (intersected) {
        return shape->material->getColor(this, scene);
    } else {
        return scene->material->getColor(this, scene);
    }
}

Point Intersection::getPosition() const {
    return ray.at(t);
}

void Intersection::print() {
        printf("Intersection %d at t=%f with shape %p (root=(%.2f, %.2f, %.2f), dir=(%.2f, %.2f, %.2f))\n",
            intersected, t, shape, ray.o[0], ray.o[1], ray.o[2], ray.dir[0], ray.dir[1], ray.dir[2]);
}
