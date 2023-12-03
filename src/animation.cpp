#include "animation.hpp"
#include "shape.hpp"
#include "image.hpp"
#include "raytracer.hpp"
#include <cstddef>


// ====================== Keyframe =========================
template <typename T>
bool Keyframe<T>::operator <(const Keyframe& other) const {
    return frame < other.frame;
}

// ====================== KeyframeSet =========================
template <typename T>
const Keyframe<T>* KeyframeSet<T>::getAdjacentForFrame(int frame, bool previous) {
    // Create a dummy keyframe for searching
    Keyframe<T> search(frame);

    typename set<Keyframe<T>>::iterator it = keyframes.find(search);
    typename set<Keyframe<T>>::iterator before, after;

    if (it != keyframes.end()) {                        // We have a keyframe for this frame.

        if (next(it) != keyframes.end()) {              // If it's not at the end we use (this, next)
            before = it;
            after = next(it);
            return previous? &(*before) : &(*after);
        } else if (it != keyframes.begin()) {           // If it's not at the beginning we use (prev, this)
            before = prev(it);
            after = it;
            return previous? &(*before) : &(*after);
        } else {                                        // Otherwise it's the only keyframe. We use (this, 0)
            before = it;
            return previous? &(*before) : nullptr;
        }
    } else {
        // We need to insert a keyframe to grab the pivot index
        keyframes.insert(search);
        it = keyframes.find(search);

        if (next(it) != keyframes.end() && it != keyframes.begin()) {
            before = prev(it);
            after = next(it);
            keyframes.erase(it);
            return previous? &(*before) : &(*after);    // If it's not the end and not the beginning, use (prev, next)
        } else if (next(it) == keyframes.end() && it == keyframes.begin()) {
            keyframes.erase(it);
            return nullptr;                             // If it's the end *and* the beginning, we don't have any keyframes
        } else if (next(it) != keyframes.end()) {
            before = next(it);
            keyframes.erase(it);
            return previous? nullptr : &(*before);          // If it's the beginning, return the next as the initial KF.
        } else if (it != keyframes.begin()) {
            before = prev(it);
            keyframes.erase(it);
            return previous? &(*before) : nullptr;
        }

    }

    assert(0);
    return 0; // Silence, clangd!

}

template class KeyframeSet<int>;
template class KeyframeSet<Real>;
template class KeyframeSet<Vector>;

template <typename T>
const Keyframe<T>* KeyframeSet<T>::getKeyframeBefore(int frame){
    return getAdjacentForFrame(frame, true);
}

template <typename T>
const Keyframe<T>* KeyframeSet<T>::getKeyframeAfter(int frame){
    return getAdjacentForFrame(frame, false);
}

template <typename T>
void KeyframeSet<T>::setFrame(int frame) {

    const Keyframe<T>* before = getKeyframeBefore(frame);
    const Keyframe<T>* after = getKeyframeAfter(frame);

    if (!before && !after) return; // No keyframes present.

    if (!before) {
        updateValue(*after, *after, 0); // First keyframe
    } else if (!after) {
        updateValue(*before, *before, 0); // Last keyframe
    } else {
        Real factor = interpolationFunction(before->frame, after->frame, frame);
        updateValue(*before, *after, factor);
    }
}

template <typename T>
void KeyframeSet<T>::addKeyframe(Keyframe<T> keyframe) {
    keyframes.erase(keyframe);
    keyframes.insert(keyframe);
}

template <typename T>
void KeyframeSet<T>::addKeyframe(int frame, T value) {
    Keyframe<T> key(frame, value);
    keyframes.erase(key);
    keyframes.insert(key);
}

// ====================== Translator =========================

void Translator::updateValue(Keyframe<Point> before, Keyframe<Point> after, Real factor) {
    Point pos = (before.value * (1 - factor)) + (after.value * factor);
    target->translate(pos - currentPosition);
    currentPosition = pos;
}

// ====================== Rotator =========================

void Rotator::updateValue(Keyframe<Real> before, Keyframe<Real> after, Real factor) {
    Real angle = (before.value * (1 - factor)) + (after.value * factor);
    target->rotate(axis, angle - currentAngle);
    currentAngle = angle;
}

// ====================== TurnTable =========================

void TurnTable::updateValue(Keyframe<Real> before, Keyframe<Real> after, Real factor) {
    Real angle = (before.value * (1 - factor)) + (after.value * factor);
    target->rotate(origin, axis, angle - currentAngle);
    currentAngle = angle;
}

// ====================== ValueAnimator =========================
template <typename T>
void ValueAnimator<T>::updateValue(Keyframe<T> before, Keyframe<T> after, Real factor) {
    *target = (before.value * (1 - factor)) + (after.value * factor);
    if (owner != nullptr) owner->update();
}

template class ValueAnimator<int>;
template class ValueAnimator<Real>;
template class ValueAnimator<Vector>;


// ====================== VisibleAnimator =========================
void VisibleAnimator::updateValue(Keyframe<bool> before, Keyframe<bool> after, Real factor) {
    bool shouldBePresent = before.value;
    bool isPresent = group->contains(shape);
    if (shouldBePresent and not isPresent) group->addShape(shape);
    if (isPresent and not shouldBePresent) group->removeShape(shape);
}

// ====================== PinAnimation =========================
template <typename T>
void PinAnimation<T>::setFrame(int frame) {
    if (frame >= startFrame && frame <= stopFrame) {
        *target = *source;
        if( owner != nullptr ) owner->update();
    }
}
template class PinAnimation<int>;
template class PinAnimation<Real>;
template class PinAnimation<Vector>;
// =================== Animation =========================

void Animator::addAnimation(Animation* anim) {
    animations.push_back(anim);
}
void Animator::setFrame(int frameNum) {
    for (int i = 0; i < animations.size(); ++i) {
        animations[i]->setFrame(frameNum);
    }
}

void Animator::render(Camera *camera, Scene* scene, string path, int width, int height, int startFrame, int stopFrame, int step) {
    Image img(width, height);
    Image depthMap(width, height);

    for(int f = startFrame; f <= stopFrame; f += step) {
        if (stopFrame - startFrame > 1)
            fprintf(stderr, "\rRendering frame %d (%.2f%%)", f, (float) (f - startFrame) * 100 / (stopFrame - startFrame));

        setFrame(f);
        scene->shapes.updateBoundingBox();
        if (startFrame == stopFrame) {
            RayTracer::rayTrace(img, depthMap, camera, scene, true);
        } else {
            RayTracer::rayTrace(img, depthMap, camera, scene);
        }

        char i_buffer[256];
        sprintf(i_buffer, "%s/frame.%04i.ppm", path.c_str(), f);

        camera->processDepthMap(img, depthMap);
        img.savePPM(i_buffer);

    }

}
