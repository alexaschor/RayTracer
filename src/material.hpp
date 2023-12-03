#ifndef MATERIAL_H
#define MATERIAL_H

#include "SETTINGS.hpp"
#include "color.hpp"

class Intersection;
class Scene;
class Shape;
class Add;
class Multiply;
class Texture;
class ConstMix;

using namespace std;

class Material {
public:
    virtual Color getColor(const Intersection* i, const Scene* scene) const = 0;
    virtual ~Material() {};
    Add operator +( const Material& other );
    Multiply operator *( const Material& other );
    ConstMix operator *( Real factor );
};

class Diffuse: public Material {
public:
    Material *color;
    Diffuse(Material *color): color(color) {};
    ~Diffuse() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Specular: public Material {
public:
    Material *color;
    Real phongExponent;
    Specular(Material *color, Real phongExponent): color(color), phongExponent(phongExponent) {};
    ~Specular() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Phong: public Material {
public:
    Material *diffuseColor;
    Material *specularColor;
    Real phongExponent;
    Phong(Material *color, Real phongExponent): diffuseColor(color), specularColor(color), phongExponent(phongExponent) {};
    Phong(Material *diffuseColor, Material *specularColor, Real phongExponent): diffuseColor(diffuseColor),
          specularColor(specularColor), phongExponent(phongExponent) {};
    ~Phong() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Mirror: public Material {
public:
    Material *color;
    int maxBounces;
    Mirror(Material *color, int maxBounces): color(color), maxBounces(maxBounces) {};
    ~Mirror() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Glossy: public Material {
public:
    Material *color;
    int maxBounces;
    int numSamples;
    Real roughness;
    Glossy(Material *color, int maxBounces, int numSamples, Real roughness): color(color), maxBounces(maxBounces), numSamples(numSamples), roughness(roughness) {};
    ~Glossy() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Glass: public Material {
public:
    Material *color;
    Real ior;
    int maxBounces;
    Glass(Material *color, Real ior, int maxBounces): color(color), ior(ior), maxBounces(maxBounces) {};
    ~Glass() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Skybox: public Material {
public:
    Texture *texture;
    Skybox(Texture *texture): texture(texture) {}
    ~Skybox() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Add: public Material {
public:
    vector<const Material *> components;
    Add();
    void addComponent(const Material * material);
    ~Add();
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Mix: public Material {
private:
    const Material *matA;
    const Material *matB;
    const Material *factorMat;
    Real factor;
    Real bias;
public:
    Mix(const Material *matA, const Material *matB, const Material *factorMat)
        : matA(matA), matB(matB), factorMat(factorMat), factor(1), bias(0) {};
    Mix(const Material *matA, const Material *matB, const Material *factorMat, Real factor, Real bias)
        : matA(matA), matB(matB), factorMat(factorMat), factor(factor), bias(bias) {};
    ~Mix() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class Multiply: public Material {
public:
    const Material *matA;
    const Material *matB;
    Multiply(const Material *matA, const Material *matB): matA(matA), matB(matB) {};
    ~Multiply() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class ConstMix: public Material {
private:
    Material *factorMat;
    Mix *mixer;

public:
    ConstMix(Material *matA, Material *matB, Real factor);
    ConstMix(Material *matA, Material *matB, Color factor);
    ~ConstMix();
    Color getColor(const Intersection* i, const Scene* scene) const;

};

// ===========================================================================
// ============================ FACTOR MATERIALS =============================
// ===========================================================================


class FactorMaterial: public Material {
public:
    virtual ~FactorMaterial() {};
    virtual Real getFactor(const Intersection* i, const Scene* scene) const = 0;
    Color getColor(const Intersection* i, const Scene* scene) const {
        Real factor = getFactor(i, scene);
        return Color(factor, factor, factor);
    }
};


class SchlickReflectance: public FactorMaterial {
public:
    Real ior;
    SchlickReflectance(Real ior): ior(ior) {};
    ~SchlickReflectance() {};
    Real getFactor(const Intersection* i, const Scene* scene) const;
};


// ===========================================================================
// =========================== COMPOSITE MATERIALS ===========================
// ===========================================================================

class Fresnel: public Material {
private:
    Material *glassColor;
    Material *mirrorColor;

    Glass glass;
    Mirror mirror;
    SchlickReflectance ref;
    Mix mixer;

public:
    Fresnel(Material *glassColor, Material *mirrorColor, Real ior, int maxBounces):
        glassColor(glassColor), mirrorColor(mirrorColor),
        // Initialize colortures
        glass(Glass(glassColor, ior, maxBounces)),
        mirror(Mirror(mirrorColor, maxBounces)),
        ref(SchlickReflectance(ior)),
        // Initialize Mix material
        mixer(Mix(&mirror, &glass, &ref)) {};

    Fresnel(Material *color, Real ior, int maxBounces):
        glassColor(color), mirrorColor(color),
        // Materials
        glass(Glass(glassColor, ior, maxBounces)),
        mirror(Mirror(mirrorColor, maxBounces)),
        ref(SchlickReflectance(ior)),
        // Mix mat
        mixer(Mix(&mirror, &glass, &ref)) {};

    Color getColor(const Intersection* i, const Scene* scene) const {
        return mixer.getColor(i, scene);
    }

};

// ===========================================================================
// ============================ TESTING MATERIALS ============================
// ===========================================================================

class SurfaceNormal: public Material {
public:
    SurfaceNormal() {};
    ~SurfaceNormal() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class RayDirection: public Material {
public:
    RayDirection() {};
    ~RayDirection() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class RayXVal: public Material {
public:
    Color color;
    RayXVal(Color color): color(color) {};
    ~RayXVal() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class RayXAbs: public Material {
public:
    Color color;
    RayXAbs(Color color): color(color) {};
    ~RayXAbs() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class RayYVal: public Material {
public:
    Color color;
    RayYVal(Color color): color(color) {};
    ~RayYVal() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};

class RayYAbs: public Material {
public:
    Color color;
    RayYAbs(Color color): color(color) {};
    ~RayYAbs() {};
    Color getColor(const Intersection* i, const Scene* scene) const;
};
#endif
