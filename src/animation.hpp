#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "SETTINGS.hpp"
#include "rtmath.hpp"
#include <algorithm>
#include <set>
#include <iostream>

class Shape;
class ShapeGroup;
class Scene;
class Camera;

using namespace std;

class Animatable {
public:
    virtual void update() {};
};

template <typename T>
class Keyframe {
public:
    int frame;
    mutable T value;
    Keyframe(int frame): frame(frame) {}
    Keyframe(int frame, T value): frame(frame), value(value) {}

    bool operator <(const Keyframe& other) const;

    static Real lerp(int startFrame, int endFrame, int frame) {
        if (frame < startFrame) return 0;
        if (frame > endFrame) return 1;
        return (Real) (frame - startFrame) / (endFrame - startFrame);
    }
};


class Animation {
public:
    virtual void setFrame(int frame) = 0;
};

template <typename T>
class KeyframeSet: public Animation {
private:
    const Keyframe<T>* getAdjacentForFrame(int frame, bool previous);
public:
    virtual void setFrame(int frame);
    virtual void addKeyframe(Keyframe<T> keyframe);
    virtual void addKeyframe(int frame, T value);
    virtual void updateValue(Keyframe<T> before, Keyframe<T> after, Real factor) = 0;
    Real (*interpolationFunction)(int, int, int) = &InterpolationFunctions::smoothstep;
protected:
    set<Keyframe<T>> keyframes;
    virtual const Keyframe<T>* getKeyframeBefore(int frame);
    virtual const Keyframe<T>* getKeyframeAfter(int frame);
};

class Translator: public KeyframeSet<Point> {
private:
    Shape *target;
    Point currentPosition;
public:
    Translator(Shape* target, Point currentPosition): target(target), currentPosition(currentPosition) {};
    virtual void updateValue(Keyframe<Point> before, Keyframe<Point> after, Real factor);
};

class Rotator: public KeyframeSet<Real> {
private:
    Shape *target;
    Vector axis;
    Real currentAngle;
public:
    Rotator(Shape* target, Vector axis, Real currentAngle): target(target), axis(axis), currentAngle(currentAngle) {};
    virtual void updateValue(Keyframe<Real> before, Keyframe<Real> after, Real factor);
};


class TurnTable: public KeyframeSet<Real> {
private:
    Shape *target;
    Point origin;
    Vector axis;
    Real currentAngle;
public:
    TurnTable(Shape* target, Point origin, Vector axis, Real currentAngle): target(target), origin(origin), axis(axis), currentAngle(currentAngle) {};
    virtual void updateValue(Keyframe<Real> before, Keyframe<Real> after, Real factor);
};

template <typename T>
class ValueAnimator: public KeyframeSet<T> {
private:
    T* target;
    Animatable *owner;
public:
    ValueAnimator(T* target, Animatable *owner): target(target), owner(owner) {};
    ValueAnimator(T* target): target(target), owner(nullptr) {};
    virtual void updateValue(Keyframe<T> before, Keyframe<T> after, Real factor);
};

class VisibleAnimator: public KeyframeSet<bool> {
private:
    Shape* shape;
    ShapeGroup* group;
public:
    VisibleAnimator(Shape* shape, ShapeGroup* group): shape(shape), group(group) {};
    virtual void updateValue(Keyframe<bool> before, Keyframe<bool>, Real factor);
};


template <typename T>
class PinAnimation: public Animation {
private:
    T* target;
    T* source;
    int startFrame;
    int stopFrame;

    Animatable *owner;
public:
    PinAnimation(T* target, T* source, int startFrame, int stopFrame, Animatable* owner)
        : target(target), source(source), startFrame(startFrame), stopFrame(stopFrame), owner(owner) {}
    PinAnimation(T* target, T* source, int startFrame, int stopFrame)
        : target(target), source(source), startFrame(startFrame), stopFrame(stopFrame), owner(nullptr){}
    void setFrame(int frame);
};

class Animator {
private:
    vector<Animation *> animations;
public:
    void addAnimation(Animation* anim);
    void setFrame(int frameNum);
    void render(Camera *camera, Scene* scene, string path, int width, int height, int startFrame, int stopFrame, int step);
};

#endif
