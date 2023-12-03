#ifndef SCENE_H
#define SCENE_H

#include "shape.hpp"
#include "material.hpp"
#include "texture.hpp"

class Scene {
public:
    Scene(Material *material) {
        shapes = ShapeGroup();
        lights = LightGroup();
        this->material = material;
    };
    ~Scene() {};
    ShapeGroup shapes;
    LightGroup lights;
    Material *material;

    vector<Material *> previzMats;
    void makePreviz() {
        for (int i = 0; i < shapes.members.size(); ++i) {
            SolidColor *c = new SolidColor((Real) rand() / RAND_MAX, (Real) rand() / RAND_MAX, (Real) rand() / RAND_MAX);
            shapes[i]->setMaterial(c);
            previzMats.push_back(c);
        }
    }

    void freePreviz() {
        for (int i = 0; i < previzMats.size(); ++i) {
            delete previzMats[i];
        }
    }
};

#endif
