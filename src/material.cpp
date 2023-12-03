#include "material.hpp"
#include "texture.hpp"
#include "SETTINGS.hpp"
#include "intersection.hpp"
#include "scene.hpp"
#include "light.hpp"
#include "plane.hpp"

using namespace std;

// ======== Material ==========
Add Material::operator +( const Material& other ) {
    Add out = Add();
    out.addComponent(this);
    out.addComponent(&other);
    return out;
}

ConstMix Material::operator *( Real factor ) {
    ConstMix out(this, nullptr, factor);
    return out;
}


Multiply Material::operator *( const Material& other ) {
    Multiply out(this, &other);
    return out;
}


// ======== Diffuse ========
Color Diffuse::getColor(const Intersection* i, const Scene* scene) const {
    Color sum(0,0,0);
    for (int x = 0; x < scene->lights.members.size(); ++x) {
        Light *light = scene->lights.members[x];
        // Get vector to light
        Vector l = (light->getPositionForIntersection(i) - i->getPosition()).normalized();

        Real factor = l.dot(i->normal);

        if (factor > 0) {
            Color lightColor = factor * light->getLightForIntersection(*i, scene);
            sum += lightColor;
        }
    }
    return sum.cwiseProduct(color->getColor(i, scene));
}

// ======== Specular ========

Color Specular::getColor(const Intersection* i, const Scene* scene) const {
    Color sum(0,0,0);
    for (int x = 0; x < scene->lights.members.size(); ++x) {
        Light *light = scene->lights.members[x];
        // Get vector to light and normal vector, then calculate reflection
        Vector l = (light->getPositionForIntersection(i) - i->getPosition()).normalized();
        Vector n = i->normal;
        Vector r = -l + 2 * (n.dot(l)) * n;

        // Get vector to eye;
        Vector e = - (i->ray).direction.normalized();

        Real factor = r.dot(e);
        if (factor > 0) {
            Color lightColor = pow(factor, phongExponent) * light->getLightForIntersection(*i, scene);
            sum += lightColor;
        }
    }
    return sum.cwiseProduct(color->getColor(i, scene));
}

// ============== Phong (combined, optimized Diffuse + Specular) ==============
Color Phong::getColor(const Intersection *i, const Scene *scene) const {
    Color sum(0,0,0);
    for (int x = 0; x < scene->lights.members.size(); ++x) {
        Light *light = scene->lights.members[x];

        // Get vector to light and normal vector, then calculate reflection
        Vector l = (light->getPositionForIntersection(i) - i->getPosition()).normalized();
        Vector n = i->normal;
        Vector r = -l + 2 * (n.dot(l)) * n;

        // Get vector to eye;
        Vector e = - (i->ray).direction.normalized();

        Real diffuseFactor = l.dot(i->normal);
        if (diffuseFactor > 0) {
            Color lightColor = diffuseFactor * light->getLightForIntersection(*i, scene);
            sum += lightColor.cwiseProduct(diffuseColor->getColor(i, scene));
        }

        Real specularFactor = r.dot(e);
        if (specularFactor > 0) {
            Color lightColor = pow(specularFactor, phongExponent) * light->getLightForIntersection(*i, scene);
            sum += lightColor.cwiseProduct(specularColor->getColor(i, scene));
        }
    }
    return sum;

}

// ============== Mirror =================

Color Mirror::getColor(const Intersection *i, const Scene *scene) const {
    // Get vector to eye and normal vector, then calculate reflection
    Vector r = RayFunctions::reflect(i->ray.dir, i->normal);

    Ray reflected(i->getPosition(), r);
    Intersection reflInter(reflected);


    if (i->DEBUG) PRINT("REFLECTING?");

    if (i->bouncesLeft == -1) {
        reflInter.bouncesLeft = maxBounces;
    } else if (i->bouncesLeft == 0){
        return Color(0,0,0);
    } else {
        reflInter.bouncesLeft = (i->bouncesLeft - 1);
    }

    if (i->DEBUG) reflInter.DEBUG = true;

    scene->shapes.intersect(reflInter);

    if (i->DEBUG) PRINT("REFLECTED");

    return reflInter.getColor(scene).cwiseProduct(color->getColor(i, scene));
}

// =============== Glossy =================
Color Glossy::getColor(const Intersection *i, const Scene *scene) const {
    // Get vector to eye and normal vector, then calculate reflection
    Vector r = RayFunctions::reflect(i->ray.dir, i->normal);

    Ray reflected(i->getPosition(), r);
    Intersection reflInter(reflected);

    if (i->bouncesLeft == -1) {
        reflInter.bouncesLeft = maxBounces;
    } else if (i->bouncesLeft == 0){
        return Color(0,0,0);
    } else {
        reflInter.bouncesLeft = (i->bouncesLeft - 1);
    }

    Color sum(0,0,0);

    // Seed rand based on position

    // BUG Assumes that Real is hashable with std:hash
    Point pos(i->getPosition());
    Point rounded_pos = Point(round(pos[0] * 1000) / 1000,
                              round(pos[1] * 1000) / 1000,
                              round(pos[2] * 1000) / 1000);
    hash<Real> hasher{};

    size_t h1 = hasher(rounded_pos[0]);
    size_t h2 = hasher(rounded_pos[1]);
    size_t h3 = hasher(rounded_pos[2]);

    size_t h = (h1 ^ (h2 << 1)) ^ (h3 << 1);

    srand(h);

    for (int s = 0; s < numSamples; ++s) {
        Intersection reflSample(reflInter);
        Real du = roughness * ((Real) rand() / (Real) RAND_MAX) - (roughness/2);
        Real dv = roughness * ((Real) rand() / (Real) RAND_MAX) - (roughness/2);

        reflSample.ray.direction += (du * i->tangent) + (dv * i->bitangent);
        scene->shapes.intersect(reflSample);
        Color c = reflSample.getColor(scene);
        // PRINTV3(c);
        sum += c;
    }

    sum /= numSamples;

    // PRINTV3(sum);

    return sum.cwiseProduct(color->getColor(i, scene));
}


// ============== Glass =================

Color Glass::getColor(const Intersection *i, const Scene *scene) const {
    // Get ray in and normal vector, then calculate refraction
    Vector in = (i->ray).direction.normalized();
    Vector n = i->normal;

    Real ior_ratio = 1.0 / ior;
    if (n.dot(in) > 0) {
        ior_ratio = ior;
        n = -n;
    }

    Real cosTheta = (-in).dot(n);

    Vector outPerp = ior_ratio * (in + cosTheta * n);

    Real outParaDiscrim = abs(1 - outPerp.squaredNorm());

    Vector out;

    if (outParaDiscrim < 0) {
        if(i->DEBUG) printf("glass TIR: reflect\n");
        out = RayFunctions::reflect(in, n);
    } else {
        Vector outPara = -sqrt(outParaDiscrim) * n;
        out = outPerp + outPara;
    }

    Ray rayOut(i->getPosition(), out);
    Intersection refrInter(rayOut);
    if(i->DEBUG) refrInter.DEBUG = true;

    if (i->bouncesLeft == -1) {
        refrInter.bouncesLeft = maxBounces;
    } else if (i->bouncesLeft == 0){
        if(i->DEBUG) printf("glass bouncemax: to black\n");
        return Color(0,0,0);
    } else {
        refrInter.bouncesLeft = (i->bouncesLeft - 1);
    }

    if (i->DEBUG) printf("recursing...");
    scene->shapes.intersect(refrInter);

    return refrInter.getColor(scene).cwiseProduct(color->getColor(i, scene));
}




Color Skybox::getColor(const Intersection* i, const Scene* scene) const {
    Vector dir = i->ray.direction;

    // Constant skybox image corners - bottom right corners
    Vec2 top_origin(2.0/4, 0.0/3);
    Vec2 bot_origin(2.0/4, 2.0/3);

    Vec2 back_origin(0.0/4, 1.0/3);
    Vec2 front_origin(2.0/4, 1.0/3);

    Vec2 left_origin(1.0/4, 1.0/3);
    Vec2 right_origin(3.0/4, 1.0/3);

    SolidColor null_mat(1,0,1);

    //TODO allow custom coord system

    Plane front(Point(0,0,1), Vector(0,0,1), 2, 2, Vector(-1, 0, 0), &null_mat, true);
    Plane back(Point(0,0,-1), Vector(0,0,-1), 2, 2, Vector(1, 0, 0), &null_mat, true);

    Plane right(Point(1,0,0), Vector(1,0,0), 2, 2, Vector(0, 0, 1), &null_mat, true);
    Plane left(Point(-1,0,0), Vector(-1,0,0), 2, 2, Vector(0, 0, -1), &null_mat, true);

    Plane bot(Point(0,1,0), Vector(0,1,0), 2, 2, Vector(-1, 0, 0), &null_mat, true);
    Plane top(Point(0,-1,0), Vector(0,1,0), 2, 2, Vector(1, 0, 0), &null_mat, true);

    Vec2 texCoords, origin;

    Ray new_ray(Point(0,0,0), i->ray.direction);

    Intersection si(new_ray);

    if (front.intersect(si)) {
        origin = front_origin;
    } else if (back.intersect(si)) {
        origin = back_origin;
    } else if (left.intersect(si)) {
        origin = left_origin;
    } else if (right.intersect(si)) {
        origin = right_origin;
    } else if (top.intersect(si)) {
        origin = top_origin;
    } else if (bot.intersect(si)) {
        origin = bot_origin;
    }

    texCoords = origin + (Vec2(si.u, si.v).cwiseProduct(Vec2(1.0/8, 1.0/6)));

    return texture->at(texCoords[0], texCoords[1]);

}


// ======== Factor Materials =========
Real SchlickReflectance::getFactor(const Intersection *i, const Scene *scene) const {
    Vector in = (i->ray).direction.normalized();
    Vector n = i->normal;

    if (n.dot(in) > 0) {
        n = -n;
    }

    Real cosTheta = (-in).dot(n);

    Real r_null = pow((1-ior)/(1 + ior), 2);

    return r_null + (1 - r_null) * pow(1 - cosTheta, 5);

}


// ======== Add ========
Add::Add() {
    components = vector<const Material *>();
}

Add::~Add() {}

void Add::addComponent(const Material *material) {
    components.push_back(material);
}

Color Add::getColor(const Intersection* i, const Scene* scene) const {
    Color sum(0,0,0);
    for (int x = 0; x < components.size(); ++x) {
        sum += components[x]->getColor(i, scene);
    }
    return sum;
}


// ======== Mix ========

Color Mix::getColor(const Intersection *i, const Scene *scene) const {
    Color facColor = (factorMat->getColor(i, scene)).cwiseProduct(Color(factor, factor, factor)) + Color(bias, bias, bias);


    if ((facColor[0] >= 1) && (facColor[1] >= 1) && (facColor[2] >= 1)) {
        return (matA == nullptr)? Color(0,0,0) : matA->getColor(i, scene);
    } else if ((facColor[0] <= 0) && (facColor[1] <= 0) && (facColor[2] <= 0)) {
        return (matB == nullptr)? Color(0,0,0) : matB->getColor(i, scene);
    } else {
        Color factorInv = Color(1,1,1) - facColor;
        Color a = (matA == nullptr)? Color(0,0,0) : matA->getColor(i, scene);
        Color b = (matB == nullptr)? Color(0,0,0) : matB->getColor(i, scene);
        return facColor.cwiseProduct(a) + factorInv.cwiseProduct(b);
    }
}


// =========== Multiply ==============

Color Multiply::getColor(const Intersection *i, const Scene *scene) const {
    return this->matA->getColor(i, scene).cwiseProduct(this->matB->getColor(i, scene));
}

// =========== ConstMix =============

ConstMix::ConstMix(Material *matA, Material *matB, Real factor) {
    factorMat = new SolidColor(factor, factor, factor);
    mixer = new Mix(matA, matB, factorMat);
}

ConstMix::~ConstMix(){
    delete factorMat;
    delete mixer;
}

ConstMix::ConstMix(Material *matA, Material *matB, Color factor) {
    factorMat = new SolidColor(factor);
    mixer = new Mix(matA, matB, factorMat);
}

Color ConstMix::getColor(const Intersection* i, const Scene* scene) const {
    return mixer->getColor(i, scene);
}


// ====================== TESTING MATERIALS ==============================

Color SurfaceNormal::getColor(const Intersection* i, const Scene* scene) const {
    return Color(i->normal.cwiseAbs());
}

Color RayDirection::getColor(const Intersection* i, const Scene* scene) const {
    return Color(i->ray.direction);
}

Color RayXVal::getColor(const Intersection* i, const Scene* scene) const {
    return i->ray.direction.normalized().x() * color;
}

Color RayXAbs::getColor(const Intersection* i, const Scene* scene) const {
    return abs(i->ray.direction.normalized().x()) * color;
}

Color RayYVal::getColor(const Intersection* i, const Scene* scene) const {
    return i->ray.direction.normalized().y() * color;
}

Color RayYAbs::getColor(const Intersection* i, const Scene* scene) const {
    return abs(i->ray.direction.normalized().y()) * color;
}
