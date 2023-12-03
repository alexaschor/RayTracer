#ifndef SHAPE_H
#define SHAPE_H

#include "SETTINGS.hpp"
#include "ray.hpp"
#include "intersection.hpp"
#include "material.hpp"
#include <cmath>
#include <algorithm>
#include "animation.hpp"

class Light;

using namespace std;

class ShapeGroup;

class AABB: public Eigen::AlignedBox<Real, 3> {
public:
    using AlignedBox<Real, 3>::AlignedBox;
    AABB(Vector c1, Vector c2): AlignedBox<Real, 3>(c1.cwiseMin(c2), c1.cwiseMax(c2)) {}

    Vector min() const {
        return this->corner(BottomLeftFloor);
    }
    Vector max() const {
        return this->corner(TopRightCeil);
    }

    bool doesIntersect(const Ray& ray) const {
        Real tmin = -INFINITY, tmax = INFINITY;

        Vector bmin = this->min();
        Vector bmax = this->max();

        for (int i = 0; i < 3; ++i) {
            if (ray.direction[i] != 0.0) {
                Real t1 = (bmin[i] - ray.origin[i])/ray.direction[i];
                Real t2 = (bmax[i] - ray.origin[i])/ray.direction[i];

                tmin = std::max(tmin, std::min(t1, t2));
                tmax = std::min(tmax, std::max(t1, t2));
            } else if (ray.origin[i] <= bmin[i] || ray.origin[i] >= bmax[i]) {
                return false;
            }
        }

        return tmax > tmin && tmax > 0.0;
    }
};

class Shape: public Animatable {
public:
    virtual ~Shape() {};
    virtual bool intersect(Intersection& i) const = 0;
    virtual void translate(const Vector& t) = 0;
    virtual void rotate(const Vector& axis, const Real angle) = 0;
    void rotate(const Point& origin, const Vector& axis, const Real angle) {
        Vector shift(this->origin - origin);
        translate(-shift);
        rotate(axis, angle);
        shift = AngleAxis3D(angle, axis) * shift;
        translate(shift);
    }
    virtual AABB getBoundingBox() const = 0;
    virtual void updateBoundingBox() {
        this->boundingBox = getBoundingBox();
    }
    virtual void setMaterial(Material* mat) {
        this->material = mat;
    }
    virtual bool shadowIntersect(Intersection& i) const {
        if (castShadows) return intersect(i);
        return false;
    }
    ShapeGroup operator +(Shape& other);
    Material *material;
    Point origin; //LCS origin
    AABB boundingBox;
    bool castShadows = true;
};


// Treat a group of shapes as a single shape.
class ShapeGroup : public Shape {
public:
    ShapeGroup();
    virtual ~ShapeGroup() {};
    void addShape(Shape* shape);
    void removeShape(Shape* shape);
    bool contains(Shape* shape);
    void translate(const Vector& t);
    using Shape::rotate;
    void rotate(const Vector& axis, const Real angle);
    virtual bool intersect(Intersection& i) const;
    virtual bool shadowIntersect(Intersection& i) const;
    virtual void setMaterial(Material* mat);
    virtual Shape* operator [](size_t i) const;
    AABB getBoundingBox() const;
    vector<Shape*> members;
};

// A group of only lights
class LightGroup {
public:
    LightGroup();
    ~LightGroup() {};
    void addLight(Light* light);
    vector<Light*> members;
};


#endif
