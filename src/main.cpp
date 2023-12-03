#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>

#include "SETTINGS.hpp"
#include "animation.hpp"
#include "camera.hpp"
#include "circle.hpp"
#include "cylinder.hpp"
#include "capsule.hpp"
#include "cylinderskeleton.hpp"
#include "image.hpp"
#include "intersection.hpp"
#include "light.hpp"
#include "material.hpp"
#include "normalmap.hpp"
#include "perlin.hpp"
#include "plane.hpp"
#include "ray.hpp"
#include "raytracer.hpp"
#include "scene.hpp"
#include "shape.hpp"
#include "sphere.hpp"
#include "texture.hpp"
#include "triangle.hpp"
#include "tube.hpp"
#include "csg.hpp"

using namespace std;

inline Real deg2rad(Real deg) {
    return deg * M_PI / 180.0;
}

void mainScene(int startFrame, int stopFrame){

    int width = 800;
    int height = 600;

    // PerspectiveCamera pc(Point(0,10,-10), Point(0,0,0), Vector(0,1,0), deg2rad(65), (float) width / height, 1);
    PerspectiveCamera pc(Point(-2.5,3,-6.125), Point(-2,0,0), Vector(0,1,0), deg2rad(65), (float) width / height, 1, 0.025, 0.9); // AS 0.05
    pc.samplesPerPixel = 17;
    pc.blurCompensation = true;
    // pc.samplesPerPixel = 1;
    // pc.blurCompensation = false;

    // Background + Scene
    SolidColor bg(Color(0,0,0));
    ImageTexture skyc("data/textures/skybox_mountain.ppm", 1, false);
    Skybox skybox(&skyc);
    Scene scn = Scene(&skybox);

    // Ground
    SolidColor ground_sc(Color::fromHex("#79693D"));
    ImageTexture ground_c("data/textures/sand_color.ppm", 5, true);
    ImageTexture ground_n("data/textures/sand_normal.ppm", 5, true);

    Diffuse ground_dif(&ground_c);

    Plane floor(Point(0,0,0), Vector(0,1,0), 1000, 1000, Vector(1,0,0), &ground_dif, true);

    // Lake
    PerlinTexture3D ripples(1, 0.5);
    SolidColor blue(0.9, 0.9, 1);
    Mirror water(&blue, 1);
    Circle lake(Point(-2,0,105), Vector(0,1,0), 100, Vector(1,0,0), &water);
    NormalMap lake_nm(&lake, &ripples, &scn, 0.1);

    // Skeletons
    SolidColor green(0.1,0.3,0.1);
    SolidColor red(0.3,0.1,0.1);
    Diffuse green_d(&green);
    Diffuse red_d(&red);
    CylinderSkeleton jenny("data/skeleton/02.asf", "data/skeleton/02_01.amc", &green_d, 0, Vector(1,1,-1), Point(0,0,0));
    CylinderSkeleton linda("data/skeleton/80.asf", "data/skeleton/80_12.amc", &red_d, 0, Vector(1,1,1), Point(1,0,3));

    Capsule arm(Point(-2, 3, -5.3), Point(-1, 3.25, -5.3), 0.12, Vector(1,0,0), &green_d);

    // Table
    ImageTexture wood_c("data/textures/wood_color.ppm", 1, true);
    ImageTexture wood_n("data/textures/wood_normal.ppm", 1, true);
    ImageTexture wood_s("data/textures/wood_spec.ppm", 1, true);

    Diffuse wood_dif(&wood_c);
    Specular wood_spec(&wood_c, 10);
    Multiply wood_rough(&wood_s, &wood_spec);
    Add wood_m = wood_dif + wood_rough;

    Plane table(Point(-1.5,2.5,-6.5), Vector(0,1,0), 5, 3, Vector(1,0,0), &wood_m);

    // Plate
    SolidColor offwhite(0.85,0.85,0.85);
    Diffuse offwhite_d(&offwhite);
    Glossy refl(&offwhite, 1, 3, 1); //TODO increase for final render
    ConstMix ceramic(&offwhite_d, &refl, 0.6);
    Cylinder plateBody(Point(-2.25, 2.5, -5), Vector(0,1,0), 0.6, 0.025, Vector(1,0,0), &offwhite_d);
    Circle plateTop(Point(-2.25, 2.5251, -5), Vector(0,1,0), 0.6, Vector(1,0,0), &ceramic);
    ShapeGroup plate{};
    plate.addShape(&plateBody);
    plate.addShape(&plateTop);

    // Orange
    SolidColor orange_sc(Color::fromHex("#FE7400"));
    ImageTexture orange_n("data/textures/orange_normal.ppm", 0.5, true);
    ImageTexture orange_s("data/textures/orange_spec.ppm", 0.5, true);

    Diffuse orange_dif(&orange_sc);
    Specular orange_spec(&orange_sc, 10);

    Multiply orange_spec_mix = orange_spec * orange_s;
    Add orange_m = orange_spec_mix + orange_dif;

    Sphere orange(Point(-2.5, 2.7751, -5), Vector(0,1,0), Vector(1,0,0), 0.25, &orange_m);
    NormalMap orange_nm(&orange, &orange_n, &scn, 0.2);

    // Apple
    ImageTexture apple_c("data/textures/apple_color.ppm");
    Diffuse apple_dif(&apple_c);
    Specular apple_spec(&apple_c, 10);
    Add apple_m = apple_spec + apple_dif;

    ImageTexture bite_c("data/textures/bite_color.ppm");
    Phong bite_m(&bite_c, 80);

    Sphere apple(Point(-2, 2.7752, -5.1), Vector(0,1,0), Vector(1,0,0), 0.25, &apple_m);
    Sphere apple_copy(apple);
    Sphere bite(Point(-1.95, 3, -5.2), Vector(0,1,0), Vector(1,0,0), 0.1875, &bite_m);

    CSGDifference bitten(&apple_copy, &bite);


    // Glasses
    SolidColor brown1_c(Color::fromHex("#A16900"));
    SolidColor brown2_c(Color::fromHex("#3A0F00"));
    PerlinTexture2D glasses_fac(2, 30);
    Mix glasses_c(&brown1_c, &brown2_c, &glasses_fac, 0.5, 0.25);
    Phong glasses_m(&glasses_c, 20);

    SolidColor white(1,1,1);
    Fresnel lens_m(&white, 1.5, 10);

    Vector lens_offset(0, 0, 0.125);
    Real lens_thickness = 0.045;

    Point lens_l_center(-3, 2.625, -4.975);

    Cylinder frame_lo(Point(-3, 2.625, -5), Point(-3, 2.625, -4.95), 0.125, Vector(1,0,0), &glasses_m);
    Cylinder frame_li(Point(-3, 2.625, -5.001), Point(-3, 2.625, -4.9499), 0.1, Vector(1,0,0), &glasses_m);
    CSGDifference frame_l(&frame_lo, &frame_li);

    Sphere lens_l1(lens_l_center + lens_offset, lens_offset.norm() + lens_thickness, &lens_m);
    Sphere lens_l2(lens_l_center - lens_offset, lens_offset.norm() + lens_thickness, &lens_m);
    CSGIntersection lens_l(&lens_l1, &lens_l2);

    Point lens_r_center(-3.325, 2.625, -4.975);

    Cylinder frame_ro(Point(-3.325, 2.625, -5), Point(-3.325, 2.625, -4.95), 0.125, Vector(1,0,0), &glasses_m);
    Cylinder frame_ri(Point(-3.325, 2.625, -5.001), Point(-3.325, 2.625, -4.9499), 0.1, Vector(1,0,0), &glasses_m);
    CSGDifference frame_r(&frame_ro, &frame_ri);

    Sphere lens_r1(lens_r_center + lens_offset, lens_offset.norm() + lens_thickness, &lens_m);
    Sphere lens_r2(lens_r_center - lens_offset, lens_offset.norm() + lens_thickness, &lens_m);
    CSGIntersection lens_r(&lens_r1, &lens_r2);

    Cylinder nose(Point(-3.225, 2.625, -4.975), Point(-3.1, 2.625, -4.975), 0.025, Vector(0,1,0), &glasses_m);

    Capsule arm_l(Point(-3.45, 2.625, -4.98), Point(-3.45, 2.625, -4.5), 0.02, Vector(0,1,0), &glasses_m);
    Capsule ear_l(Point(-3.45, 2.625, -4.5), Point(-3.45, 2.52, -4.4), 0.02, Vector(0,1,0), &glasses_m);
    Capsule arm_r(Point(-2.875, 2.625, -4.98), Point(-2.875, 2.625, -4.5), 0.02, Vector(0,1,0), &glasses_m);
    Capsule ear_r(Point(-2.875, 2.625, -4.5), Point(-2.875, 2.52, -4.4), 0.02, Vector(0,1,0), &glasses_m);

    ShapeGroup glasses{};
    glasses.addShape(&frame_l);
    glasses.addShape(&lens_l);
    glasses.addShape(&arm_l);
    glasses.addShape(&ear_l);
    glasses.addShape(&frame_r);
    glasses.addShape(&lens_r);
    glasses.addShape(&arm_r);
    glasses.addShape(&ear_r);
    glasses.addShape(&nose);

    // Milk
    Fresnel glass_f(&white, 2, 100);
    Mirror glass_r(&white, 10);
    ConstMix glass_m(&glass_f, &glass_r, 0.95);

    Cylinder glass_o(Point(-3, 2.5, -4), Point(-3, 3, -4), 0.175, Vector(1,0,0), &glass_m);
    Cylinder glass_i(Point(-3, 2.525, -4), Point(-3, 3.025, -4), 0.165, Vector(1,0,0), &glass_m);
    Cylinder milk(Point(-3, 2.525, -4), Point(-3, 2.9, -4), 0.165, Vector(1,0,0), &offwhite_d);

    CSGDifference glass(&glass_o, &glass_i);
    glass.castShadows = false;

    // Lights
    SoftSphereLight l1(Point(-4,10,-12), Color(0.75,0.75,0.75), true, 0.5, 5); //TODO increase for final render
    PointLight l2(Point(0,0,0), Color(0.5,0.5,0.5), true);
    SunLight l3(Color(0.23,0.23,0.23), Vector(-1, -1, 1));
    AmbientLight l4(Color(0.12, 0.12, 0.12));

    // Scene
    scn.shapes.addShape(&floor);
    scn.shapes.addShape(&lake_nm);
    scn.shapes.addShape(&table);
    scn.shapes.addShape(&plate);

    scn.shapes.addShape(&orange_nm);

    scn.shapes.addShape(&glasses);

    scn.shapes.addShape(&glass);
    scn.shapes.addShape(&milk);

    scn.shapes.addShape(&jenny);
    scn.shapes.addShape(&linda);

    scn.lights.addLight(&l1);
    scn.lights.addLight(&l2);
    scn.lights.addLight(&l3);
    scn.lights.addLight(&l4);

    // Lake ripples
    ValueAnimator<Real> rippler(&ripples.zLevel);
    rippler.addKeyframe(0,0);
    rippler.addKeyframe(300,20);

    // Skeleton movement
    ValueAnimator<int> sa(&jenny.frameIdx, &jenny);
    sa.interpolationFunction = InterpolationFunctions::lerp;
    sa.addKeyframe(0, 50);
    sa.addKeyframe(70, 350);

    ValueAnimator<int> sa2(&linda.frameIdx, &linda);
    sa2.interpolationFunction = InterpolationFunctions::lerp;
    sa2.addKeyframe(0, 50);
    sa2.addKeyframe(300, 1239);

    // Arm visibility
    VisibleAnimator arm_vis(&arm, &scn.shapes);
    arm_vis.addKeyframe(0, false);
    arm_vis.addKeyframe(70, true);

    // Arm move down and over
    Translator arm_move(&arm, Point(0,0,0));
    arm_move.addKeyframe(0,Point(0.5,0,0));
    arm_move.addKeyframe(70,Point(0.5,0,0));
    arm_move.addKeyframe(85,Point(0,0,0));

    // Pick up apple
    arm_move.addKeyframe(90,Point(0,0,0));
    arm_move.addKeyframe(105,Point(0.5,0.75,0));

    Translator apple_move(&apple, Point(0,0,0));
    apple_move.addKeyframe(90, Point(0,0,0));
    apple_move.addKeyframe(105, Point(0.5,0.75,0));

    // Swap apple
    VisibleAnimator apple_vis(&apple, &scn.shapes);
    apple_vis.addKeyframe(0, true);
    apple_vis.addKeyframe(105, false);

    VisibleAnimator bitten_vis(&bitten, &scn.shapes);
    bitten_vis.addKeyframe(0, false);
    bitten_vis.addKeyframe(105, true);

    // Move back down
    Translator bitten_move(&bitten, Point(0,0,0));
    bitten_move.addKeyframe(0, Point(0.5, 0.75, 0));
    bitten_move.addKeyframe(125, Point(0.5, 0.75, 0));
    bitten_move.addKeyframe(140, Point(0,0,0));

    arm_move.addKeyframe(125,Point(0.5,0.75,0));
    arm_move.addKeyframe(140, Point(0,0,0));

    // Apple wobble

    // FIXME for some reason rotating the whole csg object doesn't work
    TurnTable a_wobbler(&apple_copy, Point(-2, 2.5252, -5.1), Vector(-0.1, 0, -1), 0);
    a_wobbler.addKeyframe(0,0);
    a_wobbler.addKeyframe(137,0);
    a_wobbler.addKeyframe(143,deg2rad(-2));
    a_wobbler.addKeyframe(149,deg2rad(2));
    a_wobbler.addKeyframe(153,0);

    TurnTable b_wobbler(&bite, Point(-2, 2.5252, -5.1), Vector(-0.1, 0, -1), 0);
    b_wobbler.addKeyframe(0,0);
    b_wobbler.addKeyframe(137,0);
    b_wobbler.addKeyframe(143,deg2rad(-2));
    b_wobbler.addKeyframe(149,deg2rad(2));
    b_wobbler.addKeyframe(153,0);

    // Move arm away
    arm_move.addKeyframe(145, Point(0,0,0));
    arm_move.addKeyframe(155, Point(0.5, -0.1, -0.1));
    arm_vis.addKeyframe(155, false);

    // Skeleton swap
    VisibleAnimator jenny_vis(&jenny, &scn.shapes);
    jenny_vis.addKeyframe(0, true);
    jenny_vis.addKeyframe(155, false);

    // Camera move
    Translator cmove(&pc, pc.origin);
    cmove.addKeyframe(0, pc.origin);
    cmove.addKeyframe(165, pc.origin);
    cmove.addKeyframe(245, Point(-8,3,0));

    // Camera sweep
    Point trackPoint(pc.lookAt);

    ValueAnimator<Point> trackPointAnimator(&trackPoint);
    trackPointAnimator.addKeyframe(0, Point(-2,0,0));
    trackPointAnimator.addKeyframe(165, Point(-2,0,0));
    trackPointAnimator.addKeyframe(245, Point(-2,1.5,3));

    PinAnimation<Point> camLight(&l2.origin, &pc.origin, 0, 300);
    PinAnimation<Point> camLookAt(&pc.lookAt, &trackPoint, 0, 300, &pc);


    // Camera focus
    ValueAnimator<Real> cameraFocus(&pc.apertureSize, &pc);
    cameraFocus.interpolationFunction = InterpolationFunctions::lerp;
    cameraFocus.addKeyframe(0, pc.apertureSize);
    cameraFocus.addKeyframe(160, pc.apertureSize);
    cameraFocus.addKeyframe(190, 0.001);

    Animator anim{};
    anim.addAnimation(&sa);
    anim.addAnimation(&sa2);

    anim.addAnimation(&rippler);
    anim.addAnimation(&arm_vis);
    anim.addAnimation(&apple_vis);
    anim.addAnimation(&bitten_vis);
    anim.addAnimation(&jenny_vis);
    anim.addAnimation(&arm_move);
    anim.addAnimation(&apple_move);
    anim.addAnimation(&bitten_move);
    anim.addAnimation(&a_wobbler);
    anim.addAnimation(&b_wobbler);

    anim.addAnimation(&cmove);
    anim.addAnimation(&trackPointAnimator);
    anim.addAnimation(&camLight);
    anim.addAnimation(&camLookAt);
    anim.addAnimation(&cameraFocus);

    // scn.makePreviz();
    anim.render(&pc, &scn, "frames", width, height, startFrame, stopFrame, 1);
    // scn.freePreviz();
}


int main(int argc, char** argv) {

    int startFrame, stopFrame;

    if (argc < 2) {
        printf("Arrg! I need args!\n");
        exit(0);
    }

    if (argc < 3) {
        startFrame = stopFrame = atoi(argv[1]);
    } else {
        startFrame = atoi(argv[1]);
        stopFrame = atoi(argv[2]);
    }

    mainScene(startFrame, stopFrame);

    return 0;
}
