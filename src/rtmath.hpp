// Some common math functions in the raytracer
#include "SETTINGS.hpp"
#include "ray.hpp"

#ifndef MATH_H
#define MATH_H

class InterpolationFunctions {
public:
    static Real lerp(int f1, int f2, int f) {
        return (Real) (f - f1) / (f2 - f1);
    }

    static Real smoothstep(int f1, int f2, int f) {
        Real lerpFac = lerp(f1, f2, f);
        return lerpFac * lerpFac * (3 - 2 * lerpFac);
    }
};

class RayFunctions {
public:
    static Vector reflect(const Vector& in, const Vector& normal) {
        Vector e = -in.normalized();
        Vector r = -e + 2 * (normal.dot(e)) * normal;
        return r;
    }
};

#endif
