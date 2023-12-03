#ifndef TEXTURE_H
#define TEXTURE_H

#include "color.hpp"
#include "image.hpp"
#include "intersection.hpp"
#include "perlin.hpp"
#include "rtmath.hpp"

class Scene;
class Material;

class Texture: public Material {
public:
    virtual Color at(Real u, Real v) const = 0;
    virtual Color getColor(const Intersection* i, const Scene* scene) const {
        return at(i->u, i->v);
    }
};

class SolidColor: public Texture {
private:
    Color color;
public:
    SolidColor(Color c): color(c) {};
    SolidColor(Real r, Real g, Real b): color(Color(r,g,b)) {};
    ~SolidColor() {};
    Color at(Real u, Real v) const {
        return color;
    }
};

class CheckerTexture: public Texture {
private:
    Real scale;
public:
    CheckerTexture(Real scale):scale(scale){};
    ~CheckerTexture() {};
    Color at(Real u, Real v) const {
        if ((sin(scale * u) > 0 && sin(scale * v) > 0) ||
            (sin(scale * u) < 0 && sin(scale * v) < 0) ) {
            return Color(1,1,1);
        }
        return Color(0,0,0);
    }
};

class PerlinTexture2D: public Texture {
private:
    mutable Perlin perlin;
public:
    PerlinTexture2D(int octaves = 1, Real frequency = 1, Real amplitude = 1, Real persistence = 0.5, int seed=12345) {
        perlin = Perlin(octaves, frequency, amplitude, persistence, seed);
    }

    Color at(Real u, Real v) const {
        Real val = perlin.get2(u,v);
        return Color(val, val, val);
    }
};

class PerlinTexture3D: public Texture {
private:
    mutable Perlin perlin;
public:
    Real zLevel = 0;
    PerlinTexture3D(int octaves = 1, Real frequency = 1, Real amplitude = 1, Real persistence = 0.5, int seed=12345) {
        perlin = Perlin(octaves, frequency, amplitude, persistence, seed);
    }

    Color at(Real u, Real v) const {
        Real val = perlin.get(u, v, zLevel);
        return Color(val, val, val);
    }
};

class BumpToNormal: public Texture {
private:
    Texture* tex;
    Real step;
public:
    BumpToNormal(Texture* tex): tex(tex), step(0.005) {}
    BumpToNormal(Texture* tex, Real step): tex(tex), step(step) {}

    Color at(Real u, Real v) const {
        // We'll use a RH Zup coordinate system because that's what NormalMap uses
        Point a(u, v, tex->at(u, v).mean());
        Point b(u + step, v, tex->at(u + step, v).mean());
        Point c(u, v + step, tex->at(u, v + step).mean());

        Vector norm = (c-a).cross(b-a).normalized();
        norm = (norm / 2) + Vector(0.5, 0.5, 0.5);
        // if (norm.z() < 0) norm = -norm;
        return norm;
    }
};

class ImageTexture: public Texture {
public:
    Image image;
    Real scale;
    bool tile;
    Color background;

    ImageTexture(string filename): image(Image(filename)), scale(1), tile(false), background(Color::fromHex("E600FE")) {}
    ImageTexture(string filename, Real scale, bool tile): image(Image(filename)), scale(scale), tile(tile), background(Color::fromHex("E600FE")) {}
    ImageTexture(string filename, Real scale, bool tile, Color background)
        : image(Image(filename)), scale(scale), tile(tile), background(background) {}

    Color at(Real u, Real v) const {
        Real tu, tv;
        if (tile){
            tu = fmod(u / scale, 1);
            if (tu < 0) tu += 1;
            tv = fmod(v / scale, 1);
            if (tv < 0) tv += 1;
        } else {
            if (u < 0 || u > scale || v < 0 || v > scale) return background;
            tu = u / scale;
            tv = v / scale;
        }
        return *image.at(tu * (image.getWidth() - 1), (1-tv) * (image.getHeight()-1));
    }


};


// ======================= TESTING TEXTURES ====================

class UVTexture: public Texture {
public:
    Color at(Real u, Real v) const {
        return Color(u, v, 0);
    }
};

#endif
