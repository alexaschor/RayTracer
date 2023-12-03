#ifndef SETTINGS_H
#define SETTINGS_H

#include "Eigen/Dense"
#include "Eigen/Sparse"
#include "Eigen/Geometry"
#include <limits.h>

#define Real double
#define REAL_MAX numeric_limits<Real>::max()
#define REAL_MIN numeric_limits<Real>::min()

#define SUN_DISTANCE 100000

#define RAY_T_MIN 0.0001
#define SURFACE_EPS 0.0001

#define DEBUGBOOL true

#define PRINTV3(v) if (DEBUGBOOL) fprintf(stderr, "%s:%d (%s)\t| %s={%.2f, %.2f, %.2f};\n", __FILE__, __LINE__, __func__, #v, (v)[0], (v)[1], (v)[2])
#define PRINTD(x) if (DEBUGBOOL) fprintf(stderr, "%s:%d (%s)\t| %s=%f;\n", __FILE__, __LINE__, __func__, #x, x)
#define PRINTP(p) if (DEBUGBOOL) fprintf(stderr, "%s:%d (%s)\t| %s=%p;\n", __FILE__, __LINE__, __func__, #p, p)
#define PRINTI(x) if (DEBUGBOOL) fprintf(stderr, "%s:%d (%s)\t| %s=%d;\n", __FILE__, __LINE__, __func__, #x, x)
#define PRINT(x) if (DEBUGBOOL) {fprintf(stderr, "%s:%d (%s)\t| ", __FILE__, __LINE__, __func__); fprintf(stderr, x); fprintf(stderr, "\n");}
#define PRINTF(format, ...) if (DEBUGBOOL) {fprintf(stderr, "%s:%d (%s)\t| ", __FILE__, __LINE__, __func__); fprintf(stderr, format, ## __VA_ARGS__);}
#define HERE() PRINT("here!")
#define SOMETIMES() float SOMETIMES_RAND = rand() % 500; bool SOMETIMES_TRACKER_VAR = SOMETIMES_RAND == 1; if (SOMETIMES_TRACKER_VAR)
#define SCONT() if (SOMETIMES_TRACKER_VAR)
#define PAABB(x) PRINTV3(x.min()); PRINTV3(x.max())

typedef Eigen::Matrix<Real, 2, 2 > Matrix2;
typedef Eigen::Matrix<Real, 3, 3 > Matrix3;
typedef Eigen::Matrix<Real, 4, 4 > Matrix4;
typedef Eigen::Matrix<Real, 6, 6 > Matrix6;
typedef Eigen::Matrix<Real, 2, 1 > Vec2;
typedef Eigen::Matrix<Real, 3, 1 > Vec3;
typedef Eigen::Matrix<Real, 4, 1 > Vec4;
typedef Vec3 Point;
typedef Vec3 Vector;
typedef Eigen::Transform<Real, 3, Eigen::Affine> Transform3D;
typedef Eigen::Translation<Real, 3> Translation3D;
typedef Eigen::AngleAxis<Real> AngleAxis3D;

typedef Eigen::Matrix<Real, 4, 1 > Vec4;
typedef Eigen::Matrix<int, 3, 1 > Vec3I;

typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> MatrixReal;
typedef Eigen::Matrix<Real, Eigen::Dynamic, 1> VectorReal;
typedef Eigen::SparseMatrix<Real> Sparse_Matrix;
typedef Eigen::Triplet<Real> Triplet;

// Skeleton parameters
#define MOCAP_SCALE 0.2
#define MAX_BONES_IN_ASF_FILE 256
#define MAX_CHAR 1024
#define MAX_SKELS 16

#define PM_MAX_FRAMES 60000

#endif
