#include "sphere.hpp"

bool Sphere::intersect(Intersection &intersection) const {
    // Move origin of sphere to world origin, and shift over ray.
    // This simplifies the coefficients
    Ray copy(intersection.ray);
    copy.origin -= origin;

    // Calculate quadratic coeffs
    Real a = copy.direction.squaredNorm();
    Real b = 2 * copy.direction.dot(copy.origin);
    Real c = copy.origin.squaredNorm() - pow(radius, 2);

    // Now we examime the discriminant
    Real discriminant = pow(b, 2) - 4 * a * c;

    // If it's zero we return false, it didn't hit
    if (discriminant < 0) return false;

    Real t1 = (-b - sqrt(discriminant)) / (2 * a);
    Real t2 = (-b + sqrt(discriminant)) / (2 * a);

    if (t1 > RAY_T_MIN && (!intersection.intersected || t1 < intersection.t)) {
        intersection.t = t1;
    } else if (t2 > RAY_T_MIN && (!intersection.intersected || t2 < intersection.t)) {
        intersection.t = t2;
    } else {
        return false;
    }

    intersection.intersected = true;
    intersection.shape = this;
    intersection.normal = (intersection.getPosition() - origin).normalized();

    // Flip normal if necessary
    if (intersection.ray.dir.dot(intersection.normal) > 0) {
        intersection.normal *= -1;
    }

    // Calculate UV coordinates
    Point p = intersection.normal;
    auto theta = acos(-p.dot(up));
    auto phi = atan2(-p.dot(meridian), p.dot(east)) + M_PI;

    intersection.u = phi / (2*M_PI);
    intersection.v = theta / M_PI;

    intersection.tangent = up.cross(intersection.normal);
    intersection.bitangent = intersection.normal.cross(intersection.tangent);

    return true;
};

void Sphere::rotate(const Vector &axis, const Real angle) {
    AngleAxis3D trans(angle, axis);
    up = trans * up;
    meridian = trans * meridian;
    east = trans * east;

}

void Sphere::translate(const Vector &t) {
    origin += t;
}

AABB Sphere::getBoundingBox() const {
    Vector diagonal(radius, radius, radius);
    return AABB(origin - diagonal, origin + diagonal);
}
