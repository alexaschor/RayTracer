#ifndef PERLIN_H
#define PERLIN_H

#include "SETTINGS.hpp"
#include <stdlib.h>

#define SAMPLE_SIZE 1024

class Perlin
{
public:

  Perlin(int octaves = 1, Real frequency = 1, Real amplitude = 1, Real persistence = 0.5, int seed=12345)
      : octaves(octaves), frequency(frequency), amplitude(amplitude), seed(seed), start(true) {}

  Real get(Real x, Real y, Real z)
  {
    Real vec[3];
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;
    return fullNoise3D(vec);
  };

  Real get2(Real x, Real y)
  {
    Real vec[2];
    vec[0] = x;
    vec[1] = y;
    return fullNoise2D(vec);
  };

private:
  void init_perlin(int n,Real p);
  Real fullNoise2D(Real vec[2]);
  Real fullNoise3D(Real vec[3]);
  Real noise1(Real arg);
  Real noise2(Real vec[2]);
  Real noise3(Real vec[3]);
  void normalize2(Real v[2]);
  void normalize3(Real v[3]);
  void init(void);

  int octaves;
  Real frequency;
  Real amplitude;
  Real persistence;
  int seed;

  int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  Real g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
  Real g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
  Real g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  bool start;

};

#endif
