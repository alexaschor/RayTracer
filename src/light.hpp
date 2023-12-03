#ifndef LIGHT_H
#define LIGHT_H

#include "SETTINGS.hpp"
#include "intersection.hpp"
#include "shape.hpp"
#include "color.hpp"
#include "scene.hpp"

class Light: public Shape {
public:
    virtual Point getPositionForIntersection(const Intersection* i) = 0;
    virtual Color getLightForIntersection(const Intersection& i, const Scene* scn) = 0;
};

class AmbientLight: public Light {
public:
    Color color;

    AmbientLight(Color color): color(color) {}

    bool intersect(Intersection& i) const {
        return false;
    }

    Point getPositionForIntersection(const Intersection *i) {
        return i->getPosition() + i->normal;
    }

    Color getLightForIntersection(const Intersection& i, const Scene* scn) {
        Vector direction = (i.getPosition() - getPositionForIntersection(&i));
        Real tMax = direction.norm() - RAY_T_MIN;
        Ray shadowRay(origin, direction);
        Intersection shadowInter(shadowRay);
        scn->shapes.shadowIntersect(shadowInter);

        if ((shadowInter.intersected) && (shadowInter.t < tMax) && (shadowInter.t > RAY_T_MIN)){
            return Color(0,0,0);
        } else {
            return color;
        }
    }

    void translate(const Vector& t) {}
    void rotate(const Vector& axis, const Real angle) {}

    AABB getBoundingBox() const {
        return AABB(origin);
    }
};


class SunLight: public Light {
public:
    Color color;
    Vector direction;

    SunLight(Color color, Vector direction): color(color), direction(direction.normalized() * SUN_DISTANCE) {}

    bool intersect(Intersection& i) const {
        return false;
    }

    Point getPositionForIntersection(const Intersection *i) {
        return i->getPosition() - direction;
    }

    Color getLightForIntersection(const Intersection& i, const Scene* scn) {
        Vector origin = getPositionForIntersection(&i);
        Vector direction = (i.getPosition() - origin);
        Real tMax = direction.norm() - RAY_T_MIN;
        Ray shadowRay(origin, direction);
        Intersection shadowInter(shadowRay);
        scn->shapes.shadowIntersect(shadowInter);

        if ((shadowInter.intersected) && (shadowInter.t < tMax) && (shadowInter.t > RAY_T_MIN)){
            return Color(0,0,0);
        } else {
            return color;
        }
    }

    void translate(const Vector& t) {}
    void rotate(const Vector& axis, const Real angle) {}

    AABB getBoundingBox() const {
        return AABB(origin);
    }
};

class PointLight: public Light {
public:
    Vector origin;
    Color color;
    bool castShadows;

    PointLight(Vector origin, Color color): origin(origin), color(color), castShadows(true) {}
    PointLight(Vector origin, Color color, bool castShadows): origin(origin), color(color), castShadows(castShadows) {}

    bool intersect(Intersection& i) const {
        return false;
    }

    Point getPositionForIntersection(const Intersection *i) {
        return origin;
    }


    Color getLightForIntersection(const Intersection& i, const Scene* scn) {
        Point pos = getPositionForIntersection(&i);
        Vector direction = (i.getPosition() - pos);
        Real tMax = direction.norm() - RAY_T_MIN;
        Ray shadowRay(pos, direction);
        Intersection shadowInter(shadowRay);
        scn->shapes.shadowIntersect(shadowInter);

        if (castShadows && (shadowInter.intersected) && (shadowInter.t < tMax) && (shadowInter.t > RAY_T_MIN)){
            return Color(0,0,0);
        } else {
            return color;
        }
    }

    void translate(const Vector& t) {
        origin += t;
    }

    void rotate(const Vector& axis, const Real angle) {
        AngleAxis3D rot(angle, axis);
        origin = rot * origin;
    }

    AABB getBoundingBox() const {
        return AABB(origin);
    }


};

class SoftSphereLight: public PointLight {
private:
    Real radius;
    int numSamples;
public:
    SoftSphereLight(Vector origin, Color color, Real radius, int numSamples)
        : PointLight(origin, color), radius(radius), numSamples(numSamples) {}
    SoftSphereLight(Vector origin, Color color, bool castShadows, Real radius, int numSamples)
        : PointLight(origin, color, castShadows), radius(radius), numSamples(numSamples) {}

    Point getPositionForIntersection(const Intersection *i) override {
        Vector offset;
        for (int i = 0; i < 3; ++i) {
            offset[i] = (((Real) rand() / RAND_MAX) - 0.5) * radius;
        }
        offset.normalize();
        offset *= radius;
        return origin + offset;
    }

    Color getLightForIntersection(const Intersection& i, const Scene* scn) override {
        Color sum(0,0,0);
        for (int j = 0; j < numSamples; ++j) {
            Point pos = getPositionForIntersection(&i);
            Vector direction = (i.getPosition() - pos);
            Real tMax = direction.norm() - RAY_T_MIN;
            Ray shadowRay(pos, direction);
            Intersection shadowInter(shadowRay);
            scn->shapes.shadowIntersect(shadowInter);

            if (castShadows && (shadowInter.intersected) && (shadowInter.t < tMax) && (shadowInter.t > RAY_T_MIN)){
                sum += Color(0,0,0);
            } else {
                sum += color;
            }
        }
        return sum / numSamples;
    }


};

#endif
