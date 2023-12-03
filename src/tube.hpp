#ifndef TUBE_H
#define TUBE_H

#include "SETTINGS.hpp"
#include "shape.hpp"

using namespace std;

class Tube: public Shape {
private:
    Vector axis;
    Real radius;
    Real length;
    Vector u;

public:
    Tube(const Point& origin, const Vector& axis, Real radius, Real length, const Vector& u, Material *material):
        axis(axis), radius(radius), length(length)
    {
        Vector v = axis.cross(u);
        this->u = axis.cross(v);

        this->origin = origin;
        this->material = material;
    }

    Tube(const Point& origin, const Point& end, Real radius, const Vector& u, Material *material):
        radius(radius)
    {
        this->origin = origin;
        this->axis = (end - origin).normalized();
        this->length = (end-origin).norm();
        this->material = material;

        Vector v = axis.cross(u);
        this->u = axis.cross(v);
    }

    bool intersect(Intersection &i) const {

        Matrix3 rot;
        rot << u.normalized(), axis.normalized(), u.cross(axis).normalized();

        Vector newOrigin = rot.transpose() * (i.ray.origin - origin);
        Vector newDirection = rot.transpose() * i.ray.direction;

        // Now we construct a local-space ray and cylinder
        Ray lr(newOrigin, newDirection);

        Real dx = lr.dir.x();
        Real dz = lr.dir.z();
        Real ox = lr.origin.x();
        Real oz = lr.origin.z();

        Real a = dx * dx + dz * dz;
        Real b = 2 * (dx * ox + dz * oz);
        Real c = ox * ox + oz * oz - radius * radius;

        Real determinant = (b*b) - 4*a*c;

        if (determinant < 0) return false;

        Real t1 = (-b + sqrt(determinant)) / (2*a);
        Real t2 = (-b - sqrt(determinant)) / (2*a);

        if (t1 > t2) swap(t1, t2);

        // Real t1y = i.ray.at(t1).dot(axis) - origin.dot(axis);
        // Real t2y = i.ray.at(t2).dot(axis) - origin.dot(axis);

        Real t1y = lr.at(t1).y();
        Real t2y = lr.at(t2).y();

        Real t;

        if (t1 > RAY_T_MIN && t1y >= 0 && t1y <= length) {
            t = t1;
        } else if (t2 > RAY_T_MIN && t2y >= 0 && t2y <= length) {
            t = t2;
        } else {
            return false;
        }

        if (i.intersected && t > i.t) return false;

        Vector localNormal = lr.at(t);
        localNormal[1] = 0;
        localNormal = rot * localNormal;
        localNormal.normalize();


        // Flip normal if necessary
        // if (i.ray.dir.dot(i.normal) > 0) {
            // localNormal *= -1;
        // }

        i.t = t;
        i.intersected = true;
        i.shape = this;
        i.normal = localNormal;

        i.u = (i.getPosition() - origin).dot(axis) / length;
        i.v = acos(localNormal.dot(u));

        i.tangent = axis;
        i.bitangent = i.normal.cross(i.tangent);


        return true;

    }

    void rotate(const Vector &rAxis, const Real rAngle) {
        AngleAxis3D trans(rAngle, rAxis);
        axis = trans * axis;
        u = trans * u;
    }

    void translate(const Vector &t) {
        origin += t;
    }

    AABB getBoundingBox() const {
        return AABB(origin);
    }

};


#endif
