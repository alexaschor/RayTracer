#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "SETTINGS.hpp"
#include "shape.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "scene.hpp"

class RayTracer {
public:
    static void rayTrace(Image& image, Image& depthMap, Camera* camera, Scene* scene, bool status = false) {
        for (int x = 0; x < image.getWidth(); x++) {
            if (status) fprintf(stderr, "\rProgress: %.1f%%", ((float) x / image.getWidth()) * 100);
            for (int y = 0; y < image.getHeight(); y++) {
                Vec2 screenCoords((float) x / image.getWidth(), (float) y / image.getHeight());

                Color c_sum(0,0,0);
                Real d_sum = 0;

                for (int i = 0; i < camera->samplesPerPixel; ++i) {
                    Ray ray = camera->makeRay(screenCoords);
                    Vector dir = ray.direction;
                    Intersection intersection(ray);

                    scene->shapes.intersect(intersection);
                    c_sum += intersection.getColor(scene);

                    if (intersection.intersected) {
                        d_sum += intersection.t;
                    } else {
                        d_sum = -camera->samplesPerPixel;
                    }
                }

                Color *curPixel = image.at(x, y);
                *curPixel = c_sum / camera->samplesPerPixel;

                Color *curDepth = depthMap.at(x, y);
                Real depth = (d_sum / camera->samplesPerPixel);
                *curDepth = Color(depth, depth, depth);
            }
        }

        Color max = depthMap.getMaxColor();
        for (int i = 0; i < depthMap.getWidth() * depthMap.getHeight(); ++i) {
            if (*depthMap.at(i) == Color(-1,-1,-1)){
                *depthMap.at(i) = max;
            }
        }

    }
};

#endif
