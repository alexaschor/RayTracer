#ifndef NORMALMAP_H
#define NORMALMAP_H

#include "shape.hpp"
#include "intersection.hpp"
#include "SETTINGS.hpp"

class NormalMap: public Shape {
private:
    Shape *baseShape;
    Material *map;
    Scene *scene;
    Real factor;

public:
    NormalMap(Shape *baseShape, Material *map, Scene *scene, Real factor = 1): baseShape(baseShape), map(map), scene(scene), factor(factor) {};
    bool intersect(Intersection& i) const {
        if(baseShape->intersect(i)) {
            // Subtract 0.5 to allow for negative and positive offset
            if(i.DEBUG) PRINT("GETTING OFFSET");
            Vector offset = map->getColor(&i, scene) - Vector(0.5, 0.5, 0.5);
            if(i.DEBUG) PRINTV3(offset);
            // Bring back to full range
            offset *= 2 * factor;

            Vector worldSpaceOffset = offset[0] * i.tangent +
                                      offset[1] * i.bitangent +
                                      offset[2] * i.normal;

            if(i.DEBUG) PRINTV3(worldSpaceOffset);

            i.normal += worldSpaceOffset;
            i.normal.normalize();

            if(i.DEBUG) PRINTV3(i.normal);
            return true;
        }
        return false;
    };

    void translate(const Vector& t) {
        baseShape->translate(t);
    }

    void setMaterial(Material *mat) {
        baseShape->setMaterial(mat);
    }

    virtual void rotate(const Vector& axis, const Real angle) {
        baseShape->rotate(axis, angle);
    }

    AABB getBoundingBox() const {
        return baseShape->getBoundingBox();
    }

};


#endif
