#include "ray.hpp"

Ray::Ray() : origin(Point(0,0,0)), direction(Vector(0,0,0)) {}

Ray::Ray(const Point origin, const Vector direction):
  origin(origin), direction(direction.normalized()) {}

Ray::~Ray(){}

Point Ray::at(Real t) const {
  return origin + direction * t;
}
