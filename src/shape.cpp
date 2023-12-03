#include "shape.hpp"
#include "SETTINGS.hpp"
#include "material.hpp"

// ==================== SHAPEGROUP ======================

ShapeGroup::ShapeGroup() {
    members = vector<Shape*>();
};

void ShapeGroup::addShape(Shape *shape) {
    members.push_back(shape);
    updateBoundingBox();
}

void ShapeGroup::removeShape(Shape *shape) {
    members.erase(remove(members.begin(), members.end(), shape), members.end());
    updateBoundingBox();
}

bool ShapeGroup::contains(Shape* shape) {
     return (count(members.begin(), members.end(), shape) > 0);
}

bool ShapeGroup::intersect(Intersection &i) const {
    bool hit = false;

    if (boundingBox.doesIntersect(i.ray)) {
        for (int x = 0; x < members.size(); ++x) {
            if (members[x]->intersect(i)) hit = true;
        }
    }

    return hit;
}

bool ShapeGroup::shadowIntersect(Intersection& i) const {
    bool hit = false;

    if (boundingBox.doesIntersect(i.ray)) {
        for (int x = 0; x < members.size(); ++x) {
            if (members[x]->shadowIntersect(i)) hit = true;
        }
    }

    return hit;
}

void ShapeGroup::translate(const Vector &t) {
    for (int x = 0; x < members.size(); ++x) {
        members[x]->translate(t);
    }
}

void ShapeGroup::rotate(const Vector &axis, const Real angle) {
    for (int x = 0; x < members.size(); ++x) {
       members[x]->rotate(axis, angle);
    }
}

AABB ShapeGroup::getBoundingBox() const {
    Vector min, max;
    members[0]->updateBoundingBox();
    min = members[0]->getBoundingBox().min();
    max = members[0]->getBoundingBox().max();

    for (int i = 1; i < members.size(); ++i) {
        members[i]->updateBoundingBox();
        min = min.cwiseMin(members[i]->getBoundingBox().min());
        max = max.cwiseMax(members[i]->getBoundingBox().max());
    }

    AABB out = AABB(min, max);
    return out;
}


Shape* ShapeGroup::operator [](size_t i) const {
    return members[i];
}

void ShapeGroup::setMaterial(Material* mat) {
    for (int i = 0; i < members.size(); ++i) {
        members[i]->setMaterial(mat);
    }
}

// ===================== SHAPE + =========================

ShapeGroup Shape::operator +(Shape& other) {
    ShapeGroup out = ShapeGroup();
    out.addShape(this);
    out.addShape(&other);
    return out;
}


// ==================== LIGHTGROUP ======================

LightGroup::LightGroup() {
    members = vector<Light*>();
};

void LightGroup::addLight(Light *light) {
    members.push_back(light);
}
