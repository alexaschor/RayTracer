#ifndef RAY_H
#define RAY_H

#include "SETTINGS.hpp"
#include "color.hpp"


class Ray {
public:
  Point origin;
  Point o = origin;
  Vector direction;
  Vector dir = direction;

  Ray();
  Ray(const Point origin, const Vector direction);
  ~Ray();

  Point at(Real t) const;
};

#endif
