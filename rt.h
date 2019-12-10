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

#ifdef USE_EIGEN
#else
#include "linalg.h"
#endif

namespace rt {

#ifdef USE_EIGEN
#else
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

    virtual ~Material() = default;
    virtual Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const = 0;
};

struct PbrMaterial : public Material {
    double alpha;
    Vector3d baseColor;
    bool doubleSided;
    Vector3d emissiveFactor;
    std::string name;
    double metallicFactor;
    double roughnessFactor;
};

struct MetallicMaterial : public PbrMaterial {
    Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const override ;
};

struct DielectricMaterial : public PbrMaterial {
    Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const override;
};

struct LightSource : public PbrMaterial {
    Vector3d getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const override;
};

struct HitRecord {
    bool didHit = false;
    double distance;
    Vector3d point;
    Vector3d normal;
    int matIdx;
};

class Object {
public:
    virtual ~Object() = default;
    virtual bool doesHit(const Ray &ray, HitRecord &hit) const = 0;
};

struct Primitive {
    Vector3i vIndicies;
    int matIdx;
};

struct Mesh : public Object {
    std::vector<Primitive> primitives;
    std::vector<Vector3d> vertices;

    bool doesHit(const Ray &ray, HitRecord &hit) const;
};

struct Sphere : public Object {
    double radius;
    Vector3d point;
    int matIdx;

    bool doesHit(const Ray &ray, HitRecord &hit) const;
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

    Scene(const tinygltf::Model &m);
    Scene();

    const Material& getMatAtIdx(int matIdx) const;
    HitRecord findHit(const Ray &r) const;
};

struct RenderOptions {
    int horizontalResolution;
    int verticalResolution;
    int maxDepth;
    int threadCount;
    int samplesPerPixel;
};

struct RenderContext {
    Scene *scene;
    RenderOptions *options;
    Image *image;
};

void rayTrace(const RenderContext &ctx);

int test(int count);

}

#endif //PATH_TRACER_RT_H
