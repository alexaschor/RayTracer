#ifndef CAMERA_H
#define CAMERA_H

#include "SETTINGS.hpp"
#include "ray.hpp"
#include "color.hpp"
#include "shape.hpp"
#include "image.hpp"

class Camera {
public:
    virtual ~Camera() {};
    virtual Ray makeRay(Vec2 imgCoords) const = 0;
    virtual Ray makeDepthRay(Vec2 imgCoords) const {
        return makeRay(imgCoords);
    }
    int samplesPerPixel = 1;
    int depthSamplesPerPixel = 1;
    virtual void processDepthMap(Image& image, Image& depthMap) const {}
};

class PerspectiveCamera: public Camera, public Shape {
public:
    Vector up, u, v, w;
    Point origin, lookAt;
    Real width, height;
    Real near;
    Real apertureSize;
    Real focalLength;
    bool blurCompensation = false;

    PerspectiveCamera(Point origin, Point lookAt, Vector up, Real fovy, Real aspect, Real near, Real apertureSize, Real focalLength);
    Ray makeRay(Vec2 imgCoords) const;
    Ray makeDepthRay(Vec2 imgCoords) const;
    bool intersect(Intersection& i) const;
    void update();
    void translate(const Vector& t);
    void rotate(const Vector& axis, const Real angle);
    AABB getBoundingBox() const;
    virtual void processDepthMap(Image& image, Image& depthMap) const;
};


#endif
