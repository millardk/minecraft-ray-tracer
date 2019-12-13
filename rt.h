//
// Created by Keegan Millard on 2019-12-05.
//

#ifndef PATH_TRACER_RT_H
#define PATH_TRACER_RT_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <array>

#include "image.h"

#include "linalg.h"

namespace rt {

struct Material {
    Vector3d ka;
    Vector3d kd;
    Vector3d ks;
    double ns;
    Material(const Vector3d &ka, const Vector3d &kd, const Vector3d &ks, double ns): ka(ka), kd(kd), ks(ks), ns(ns) {}
    Material(): ka(Vector3d(0.2,0.2,0.2)), kd(Vector3d(0.5,0.5,0.5)), ks(Vector3d(0.8,0.8,0.8)), ns(10) {}
    Material(const Material &m) = default;
};

struct MetaMaterial {
    virtual Material getMaterialAt(double a, double b) const = 0;
};

struct BlockMaterial {

    static BlockMaterial makeUniformBlock(MetaMaterial *mat);
    static BlockMaterial makeTopAndSideBlock(MetaMaterial *top, MetaMaterial *side);

    std::array<MetaMaterial*, 6> mats;
    Material getMaterialAt(double a, double b, int side) const;
};

struct TextureMaterial : MetaMaterial {


    Material getMaterialAt(double a, double b) const override;
};

struct SolidColorMaterial : MetaMaterial {
    Material mat;
    SolidColorMaterial(const Material &mat): mat(mat) {}
    Material getMaterialAt(double a, double b) const override;
};

struct HitRecord {
    bool didHit = false;
    double distance;
    Vector3d point;
    Vector3d normal;
    int a;
    int b;
    int side;
    int type;
};

struct LightSource {
    Vector3d position;
    Vector3d emittance;
};

enum BlockSides {
    SIDE_TOP=0,SIDE_WEST=1,SIDE_NORTH=2,SIDE_EAST=3,SIDE_SOUTH=4,SIDE_BOTTOM=5
};

struct Block {
    Vector3i position;
    int type;

    Block() {}
    Block(const Vector3i &position, int type): position(position), type(type) {}

    bool doesHit(const Ray &r, HitRecord &hit) const;
};

class BoxStore {
private:
    std::vector<Block> blocks;


public:
    BoxStore(std::vector<Block> blocks);

    bool doesHit(const Ray &r, HitRecord &hit) const;
};

struct Camera {
    virtual Ray pixelRay(int r, int c, int hRes, int vRes) const = 0;
};

struct PerspectiveCamera : public Camera {
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
    Ray pixelRay(int r, int c, int hRes, int vRes) const override;
};

struct Scene {
    Vector3d ambientLight;
    BoxStore *boxStore;
    std::unordered_map<int, BlockMaterial> blockMaterials;
    std::vector<LightSource> lights;
    Camera *camera;

    Scene();

    Material getMaterial(int blockType, int side, double a, double b) const;
    HitRecord getHit(const Ray &r) const;
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
