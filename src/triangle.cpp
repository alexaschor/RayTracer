#include "triangle.hpp"

// ==================== TRIANGLE ======================

bool Triangle::intersect(Intersection &intersection) const {
    // We first do a plane intersection test
    Vector edge_a = b - a;
    Vector edge_b = c - a;

    // Calculate the normal
    Vector n = edge_a.cross(edge_b).normalized();

    Real n_dot_d = n.dot(intersection.ray.direction);

    if ( n_dot_d < SURFACE_EPS && n_dot_d > -SURFACE_EPS) {
        // Plane is parallel to ray
        return false;
    }

    // We know the ray intersects the plane.
    // Now we need to know the plane's offset along its normal:
    Real offset = n.dot(a);

    // Finally calculate the t at which the ray intersects the plane
    Real t = (offset - n.dot(intersection.ray.origin)) / n_dot_d;

    // Avoid self-intersecion
    if (t < RAY_T_MIN) return false;
    // Make sure it's the closest
    if (intersection.intersected && intersection.t < t) return false;

    // And now we compute barycentric coordinates
    Point p = intersection.ray.at(t);

    Real area_a = (b - p).cross(c - p).norm() / 2.0;
    Real area_b = (p - a).cross(c - a).norm() / 2.0;
    Real area_c = (p - a).cross(b - a).norm() / 2.0;

    Real area_all = (c - a).cross(b - a).norm() / 2.0;

    Real alpha = area_a / area_all;
    Real beta = area_b / area_all;
    Real gamma = area_c / area_all;

    Real sum = alpha + beta + gamma;

    Real bot = (0 - SURFACE_EPS);
    Real top = (1 + SURFACE_EPS);

    if (bot <= sum && sum <= top) {
        intersection.t = t;
        intersection.intersected = true;
        intersection.shape = this;
        intersection.normal = n;
        return true;
    }

    return false;

}

void Triangle::rotate(const Vector &axis, const Real angle) {
    AngleAxis3D rot(angle, axis);
    a = rot * a;
    b = rot * b;
    c = rot * c;
}

void Triangle::translate(const Vector &t) {
    a += t;
    b += t;
    c += t;
}

AABB Triangle::getBoundingBox() const {
    Vector min = a.cwiseMin(b.cwiseMin(c));
    Vector max = a.cwiseMax(b.cwiseMax(c));
    return AABB(min, max);
}
