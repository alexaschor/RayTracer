#include "circle.hpp"


Circle::Circle(const Point& origin, const Vector& normal, Real radius, Material* material): origin(origin), normal(normal), radius(radius) {
    this->material = material;
    u = v = Vector(0,0,0);
}

Circle::Circle(const Point& origin, const Vector& normal, Real radius, const Vector& u, Material* material): origin(origin), normal(normal), radius(radius) {
    this->material = material;
    v = u.cross(normal);
    this->u = v.cross(normal);
}

void Circle::translate(const Vector& t) {
    origin += t;
}
void Circle::rotate(const Vector& axis, const Real angle) {
    AngleAxis3D rot(angle, axis);
    normal = rot * normal;
    u = rot * u;
    v = rot * v;
}

bool Circle::intersect(Intersection &intersection) const {
    // Check if ray is parallel to plane
    Real n_dot_d = normal.dot(intersection.ray.direction);
    if (n_dot_d < SURFACE_EPS && n_dot_d > -SURFACE_EPS) return false;

    // Otherwise we have a t.
    // Compute plane offset
    Real offset = origin.dot(normal);
    Real t = (offset - normal.dot(intersection.ray.origin)) / n_dot_d;

    // Avoid self-intersecion
    if (t < RAY_T_MIN) return false;

    // Make sure it's the closest
    if (intersection.intersected && intersection.t < t) return false;

    // Now we check if it's outside the radius
    if ((intersection.ray.at(t) - origin).norm() > radius) return false;



    // We got a bingo!
    intersection.t = t;
    intersection.intersected = true;
    intersection.shape = this;
    intersection.normal = normal;

    // Flip normal if necessary
    if (intersection.ray.dir.dot(intersection.normal) > 0) {
        intersection.normal *= -1;
    }

    // Calculate UV coordinates
    Vector localPos = intersection.getPosition() - origin;
    intersection.u = localPos.dot(u)/2 + radius; // Start the texture at the bottom corner, off of the circle
    intersection.v = localPos.dot(v)/2 + radius; // This lets the texture continuously cover the entire circle w/o tiling.

    intersection.tangent = u;
    intersection.bitangent = v;

    return true;
}

AABB Circle::getBoundingBox() const {
    Vector min, max;
    min = origin - (u * radius) - (v * radius) - (SURFACE_EPS * normal);
    max = origin + (u * radius) + (v * radius) + (SURFACE_EPS * normal);
    return AABB(min, max);
}
