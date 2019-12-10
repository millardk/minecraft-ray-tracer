//
// Created by Keegan Millard on 2019-12-05.
//

#ifndef PATH_TRACER_RT_H
#define PATH_TRACER_RT_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "image.h"
#include "tinygltf/tiny_gltf.h"

namespace rt {

#ifdef USE_EIGEN
#else

#include "linalg.h"

typedef lin::Vector3d Vector3d;
typedef lin::Vector3i Vector3i;
typedef lin::Vector4d Vector4d;
typedef lin::Matrix4_4d Matrix4_4d;
#endif

// TODO rename o to p
struct Ray {
    Vector3d o;
    Vector3d d;

    Ray(const Vector3d &o, const Vector3d &d);
};

class Material {
public:
    static Material *makeFromGltfMaterial(const tinygltf::Material &m);

    virtual Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) = 0;
};

struct PbrMaterial : public Material {
    double alpha;
    Vector3d baseColor;
    bool doubleSided;
    std::string name;
    double metallicFactor;
    double roughnessFactor;
};

struct MetallicMaterial : public PbrMaterial {
    Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) override;
};

struct DielectricMaterial : public PbrMaterial {
    Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) override;
};

struct HitRecord {
    bool didHit;
    Vector3d p;
    Vector3d n;
    Material *m;
};

class Object {
    virtual HitRecord findHit(const Ray &) = 0;
};

struct Primitive {
    Vector3i vIndicies;
    int matIdx;
};

class Mesh : public Object {
    std::vector<Primitive> primitives;
    std::vector<Vector3d> vertices;

    HitRecord findHit(const Ray &);
};

class Sphere : public Object {
    double radius;
    Vector3d position;
    Material *mat;

    HitRecord findHit(const Ray &);
};

struct Camera {
    Vector3d focalPoint;
    Vector3d lookPoint;
    Vector3d upVector;
    double imagePlaneDistance;
    double vB1;
    double vB2;
    double hB1;
    double hB2;

    Vector3d lv;
    Vector3d hv;
    Vector3d vv;
    Vector3d ipCpt; // image plane center point

    double planeWidth; // image plane width
    double planeHeight; // image plane height

    void init();
    Ray pixelRay(int r, int c, int hRes, int vRes) const;
};

struct Scene {
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Material>> materials;
    Camera camera;

    HitRecord findHit(const Ray &r);
};

struct RenderOptions {
    int horizontalResolution;
    int verticalResolution;
    int maxDepth;
    int threadCount;
};

struct RenderContext {
    Scene *scene;
    RenderOptions *options;
    Image *image;
};

void rayTrace(const RenderContext &ctx);

}

#endif //PATH_TRACER_RT_H
