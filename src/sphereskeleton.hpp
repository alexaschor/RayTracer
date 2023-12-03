#ifndef SPHERESKELETON_H
#define SPHERESKELETON_H

#include <string>
#include "sphere.hpp"
#include "shape.hpp"
#include "Mocap/skeleton.h"
#include "Mocap/displaySkeleton.h"
#include "Mocap/motion.h"

class SphereSkeleton: public ShapeGroup {
private:
    Skeleton *skeleton;
    Motion *motion;
    DisplaySkeleton *displayer;
    Material *material;
    int curFrameIdx;
public:
    int frameIdx; // public for animator access

    SphereSkeleton(string skeletonFilename, string motionFilename, Material* material, int initialFrame = 0): material(material) {
        // DisplaySkeleton
        displayer = new DisplaySkeleton();

        // Skeleton
        skeleton = new Skeleton(skeletonFilename.c_str(), MOCAP_SCALE);
        skeleton->setBasePosture();
        displayer->LoadSkeleton(skeleton);

        // Motion
        motion = new Motion(motionFilename.c_str(), MOCAP_SCALE, skeleton);
        displayer->LoadMotion(motion);
        skeleton->setPosture(*(displayer->GetSkeletonMotion(0)->GetPosture(initialFrame))); // Set initial posture

        curFrameIdx = frameIdx = initialFrame;
        updatePosition();
    }

    void clearMembers() {
        for (int i = 0; i < members.size(); ++i) {
            delete members[i];
        }
        members.clear();
    }

    void updatePosition() {
        // Clamp frameIdx
        if (frameIdx < 0) frameIdx = 0;
        if (frameIdx >= displayer->GetSkeletonMotion(0)->GetNumFrames()) frameIdx = displayer->GetSkeletonMotion(0)->GetNumFrames() - 1;

        // printf("ASSUMING POSITION %d\n", frameIdx);

        // Set posture and calculate new bone positions
        displayer->GetSkeleton(0)->setPosture(* (displayer->GetSkeletonMotion(0)->GetPosture(frameIdx)));
        displayer->ComputeBonePositions(DisplaySkeleton::BONES_AND_LOCAL_FRAMES);


        // Clear existing spheres
        clearMembers();

        // Now we create spheres in the shape of the skeleton.
        // First grab the bone parameters:
        vector<Matrix4>& rotations = displayer->rotations();
        vector<Matrix4>& scalings = displayer->scalings();
        vector<Vec4>& translations = displayer->translations();
        vector<float>& lengths = displayer->lengths();

        int numBones = rotations.size();

        // Now we iterate, disregarding the first bone (the origin)
        for (int i = 1; i < numBones; ++i) {
            Matrix4& rotation = rotations[i];
            Matrix4& scaling = scalings[i];
            Vec4& translation = translations[i];

            Vec4 leftVertex(0,0,0,1);
            Vec4 rightVertex(0,0,lengths[i],1);

            leftVertex = rotation * scaling * leftVertex + translation;
            rightVertex = rotation * scaling * rightVertex + translation;

            // Get the direction we need to interpolate in
            Vector direction = (rightVertex - leftVertex).head<3>();
            const float magnitude = direction.norm();
            direction *= 1.0 / magnitude;

            // How many spheres, and what spacing?
            const float sphereRadius = 0.1;
            const int totalSpheres = magnitude / (2.0 * sphereRadius);
            const float rayIncrement = magnitude / (float)totalSpheres;

            // Reduce dimensions after transform
            Vector left3(leftVertex[0], leftVertex[1], leftVertex[2]);
            Vector right3(rightVertex[0], rightVertex[1], rightVertex[2]);

            // Store the endpoints
            members.push_back(new Sphere(left3, sphereRadius, material));
            members.push_back(new Sphere(right3, sphereRadius, material));

            // Store the intermediate spheres
            for (int j = 0; j < totalSpheres; j++) {
                Point center = ((float)j + 0.5) * rayIncrement * direction + leftVertex.head<3>();
                members.push_back(new Sphere(center, sphereRadius, material));
            }

        }
    }

    void update() override {
        updatePosition();
    }

};

#endif
