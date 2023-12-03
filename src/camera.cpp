#include "camera.hpp"
#include "SETTINGS.hpp"
#include <cmath>

PerspectiveCamera::
PerspectiveCamera(Point origin, Point lookAt, Vector up, Real fovy, Real aspect,
    Real near, Real apertureSize, Real focalLength)
    : up(up), lookAt(lookAt), apertureSize(apertureSize), focalLength(focalLength)
{
    this->near = near;
    this->origin = origin;

    height = 2.0 * tan(fovy / 2.0);
    width = height * aspect;

    update();
}

void PerspectiveCamera::update() {
    Vector gaze = (lookAt - origin).normalized();

    w = -gaze;
    u = up.cross(w).normalized();
    v = w.cross(u);
}

Ray PerspectiveCamera::makeRay(Vec2 imgCoords) const {
    Real x = imgCoords.x();
    Real y = imgCoords.y();

    Real dx = ((Real) rand() / RAND_MAX) * apertureSize;
    Real dy = ((Real) rand() / RAND_MAX) * apertureSize;

    Vector wiggle(dx,dy,0);

    Point throughPoint = origin +
        (
            -((2 * x - 1) * (width / 2.0) * u) -
            ((2 * y - 1) * (height / 2.0) * v) -
            near * w
        ) * focalLength;

    Point source = origin + wiggle;

    Vector dir = throughPoint - source;

    return Ray(source, dir.normalized());
}


// FIXME code duplicated here, abstract away
Ray PerspectiveCamera::makeDepthRay(Vec2 imgCoords) const {
    Real x = imgCoords.x();
    Real y = imgCoords.y();

    Point throughPoint = origin +
        (
            -((2 * x - 1) * (width / 2.0) * u) -
            ((2 * y - 1) * (height / 2.0) * v) -
            near * w
        ) * focalLength;

    Point source = origin;

    Vector dir = throughPoint - source;

    return Ray(source, dir.normalized());
}

void PerspectiveCamera::processDepthMap(Image &image, Image &depthMap) const {
    if (blurCompensation) {
        for (int x = 0; x < image.getWidth(); ++x) {
            for (int y = 0; y < image.getHeight(); ++y) {
                Real distFromFoc = pow(abs((*depthMap.at(x, y))[0] - focalLength), 0.5); // Depth is same for three channels
                *depthMap.at(x,y) = Color(distFromFoc, distFromFoc, distFromFoc);
            }
        }

        depthMap.normalize();

        // depthMap.boxBlurInPlace(2);

        depthMap.savePPM("depthmap.ppm");
        image.savePPM("before.ppm");

        Image imgCopy(image); // Image copy constructor makes a deep copy

        for (int x = 0; x < image.getWidth(); ++x) {
            for (int y = 0; y < image.getHeight(); ++y) {
                Real blurFactor = (*depthMap.at(x,y))[0];
                Real imageUnitSize = (Real) (image.getWidth() + image.getHeight()) / 2;
                int radius = blurFactor * imageUnitSize * apertureSize;

                *image.at(x,y) = imgCopy.getBoxBlurredColor(x, y, radius);
            }

            image.savePPM("after.ppm");

        }
    }

}

bool PerspectiveCamera::intersect(Intersection& i) const {
    return false;
}

void PerspectiveCamera::translate(const Vector& t) {
    origin += t;
}

void PerspectiveCamera::rotate(const Vector& axis, const Real angle) {
    AngleAxis3D trans(angle, axis);
    u = trans * u;
    v = trans * v;
    w = trans * w;
}

AABB PerspectiveCamera::getBoundingBox() const {
    return AABB(origin);
}
