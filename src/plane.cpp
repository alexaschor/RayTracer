#include "plane.hpp"
#include "Eigen/src/Core/util/Constants.h"
#include "SETTINGS.hpp"

// ==================== PLANE ======================

Plane::Plane(const Point& point, const Vector& normal, Real width, Real height, const Vector& u, Material *material, bool center)
    : normal(normal), width(width), height(height)
{
    origin = point;
    this->material = material;
    this->v = u.cross(normal);
    this->u = this->v.cross(normal);

    if(center) {
        Vector offsetU = -width * (this->u) / 2;
        Vector offsetV = -height * (this->v) / 2;
        origin = origin + offsetU + offsetV;
    }

}

void Plane::translate(const Vector& t) {
    origin += t;
}
void Plane::rotate(const Vector& axis, const Real angle) {
    AngleAxis3D rot(angle, axis);
    normal = rot * normal;
    u = rot * u;
    v = rot * v;
}

bool Plane::intersect(Intersection &intersection) const {
    Real offset = origin.dot(normal);

    // Check if ray is parallel to plane
    Real n_dot_d = normal.dot(intersection.ray.direction);
    if (n_dot_d < SURFACE_EPS && n_dot_d > -SURFACE_EPS) return false;

    // Otherwise we have a t.
    Real t = (offset - normal.dot(intersection.ray.origin)) / n_dot_d;

    // Avoid self-intersecion
    if (t < RAY_T_MIN) return false;

    // Make sure it's the closest
    if (intersection.intersected && intersection.t < t) return false;

    // Check to see if it's in bounds:
    Point hit = intersection.ray.at(t);
    Point localPos = hit - origin;
    Real hitU = localPos.dot(u);
    Real hitV = localPos.dot(v);

    if (hitU < 0 || hitU > width || hitV < 0 || hitV > height) {
        return false;
    }

    // We got a bingo!
    intersection.t = t;
    intersection.intersected = true;
    intersection.shape = this;
    intersection.normal = normal;

    // Copy over UV coords
    intersection.u = hitU;
    intersection.v = hitV;
    intersection.tangent = u;
    intersection.bitangent = v;

    return true;
}

AABB Plane::getBoundingBox() const {
    Point c1(origin + (width * u) + (height * v) + (normal * 1));
    Point c2(origin - (normal * 1));
    AABB out(c1.cwiseMin(c2), c1.cwiseMax(c2));
    return out;
}
