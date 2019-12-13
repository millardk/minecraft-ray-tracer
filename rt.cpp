//
// Created by Keegan Millard on 2019-12-05.
//

#include "rt.h"

#include <cmath>
#include <thread>
#include <random>
#include <fstream>


constexpr double MATH_PI = 3.141592653589793238462643383279502884;

using namespace rt;


Vector3d entrywiseProduct(const Vector3d &v1, const Vector3d &v2) {
    return Vector3d(
            v1[0] * v2[0],
            v1[1] * v2[1],
            v1[2] * v2[2]);
}

rt::BlockStore::BlockStore(std::vector<rt::Block> blockList) {
    for (Block b : blockList) {
        addBlock(b);
    }
}


Vector3d toVector3d(Vector3i v) {
    return Vector3d(v[0],v[1],v[2]);
}

Vector3i toVector3i(Vector3d v) {
    return Vector3i(v[0],v[1],v[2]);
}


bool ClipLine(int d, const Block &block, const Ray &r, double& f_low, double& f_high)
{
    // f_low and f_high are the results from all clipping so far. We'll write our results back out to those parameters.

    // f_dim_low and f_dim_high are the results we're calculating for this current dimension.
    double f_dim_low, f_dim_high;

    // Find the point of intersection in this dimension only as a fraction of the total vector http://youtu.be/USjbg5QXk3g?t=3m12s

    // is direction is basically 0
//    if (abs(r.d[d]) < 1e-9 ) {
//        double center = block.position[d] - r.o[d] + 0.5;
//        return center >= -0.5 && center <= 0.5;
//    }

    f_dim_low = (block.position[d] - r.o[d])/(r.d[d]);
    f_dim_high = (block.position[d]+1 - r.o[d])/(r.d[d]);

    // Make sure low is less than high
    if (f_dim_high < f_dim_low)
        std::swap(f_dim_high, f_dim_low);

    // If this dimension's high is less than the low we got then we definitely missed. http://youtu.be/USjbg5QXk3g?t=7m16s
    if (f_dim_high < f_low)
        return false;

    // Likewise if the low is less than the high.
    if (f_dim_low > f_high)
        return false;

    // Add the clip from this dimension to the previous results http://youtu.be/USjbg5QXk3g?t=5m32s
    f_low = std::max(f_dim_low, f_low);
    f_high = std::min(f_dim_high, f_high);

    if (f_low > f_high)
        return false;

    return true;
}

// Find the intersection of a line from v0 to v1 and an axis-aligned bounding box http://www.youtube.com/watch?v=USjbg5QXk3g
bool LineAABBIntersection(const Block &block, const Ray &ray, Vector3d& vecIntersection, double& flFraction)
{
//    double f_low = 0;
//    double f_high = 1;
    double f_low = 1e-9;
    double f_high = INFINITY;

    if (!ClipLine(0, block, ray, f_low, f_high))
        return false;

    if (!ClipLine(1, block, ray, f_low, f_high))
        return false;

    if (!ClipLine(2, block, ray, f_low, f_high))
        return false;

    // The formula for I: http://youtu.be/USjbg5QXk3g?t=6m24s
    vecIntersection = ray.o + ray.d * f_low;

    flFraction = f_low;

    return true;
}


bool Block::doesHit(const Ray &r, HitRecord &hit) const {
    if (!LineAABBIntersection(*this, r, hit.point, hit.distance)) {
        return false;
    }

    Vector3d center = toVector3d(position) + Vector3d(0.5,0.5,0.5);
    Vector3d toIntersection = hit.point - center;

    hit.normal = toVector3d(toVector3i(toIntersection * 2.0000001));
    hit.normal.normalize();

    return true;
}


// TODO ADD THE A AND B FUNCTIONALITY
bool BlockStore::doesHit(const Ray &r, HitRecord &hit) const {
    HitRecord cur;
    for (const auto &block : blocks) {
        if (block.doesHit(r, cur) && (!hit.didHit|| cur.distance < hit.distance) ) {
            hit = cur;
        }
    }

    double epsilon = 1e-9;
    if (hit.didHit) {

        if (abs(hit.normal[0]) > 1.0-epsilon) {
            hit.a = hit.point[2];
            hit.b = hit.point[1];

            if (hit.normal[0] > 0) {
                hit.side =  SIDE_EAST;
            } else {
                hit.side = SIDE_WEST;
            }

        } else if (abs(hit.normal[1]) > 1.0-epsilon) {
            hit.a = hit.point[0];
            hit.b = hit.point[2];

            if (hit.normal[1] > 0) {
                hit.side =  SIDE_TOP;
            } else {
                hit.side = SIDE_BOTTOM;
            }

        } else if (abs(hit.normal[2]) > 1.0-epsilon) {
            hit.a = hit.point[0];
            hit.b = hit.point[1];

            if (hit.normal[2] > 0) {
                hit.side =  SIDE_SOUTH;
            } else {
                hit.side = SIDE_NORTH;
            }

        } else {
            // maybe we hit an edge
            hit.side = SIDE_EAST;
            hit.a = 0;
            hit.b = 0;
        }

    }


    return hit.didHit;
}

void BlockStore::addBlock(const Block &b) {
    for (BlockContainer &container : blocks) {
        if (container.addBlock(b)) {
            return;
        }
    }

    BlockContainer newBlock(
            BlockContainer::getChunkPos(b.position[0],ContainerSize),
            BlockContainer::getChunkPos(b.position[2], ContainerSize),
            ContainerSize);
    newBlock.addBlock(b);
    blocks.push_back(newBlock);
}

// ******************* Camera *******************

void PerspectiveCamera::init() {
    lv = lookPoint - focalPoint; lv.normalize();
    hv = lv.cross(upVector); hv.normalize();
    vv = hv.cross(lv); vv.normalize();
    ipCpt = focalPoint + (lv * imagePlaneDistance);
    planeWidth = abs(hB2 - hB1);
    planeHeight = abs(vB2 - vB1);
}

Ray PerspectiveCamera::pixelRay(int r, int c, int hRes, int vRes) const {
    double h_amt = 0;
    double v_amt = 0;
    if (hRes > 1) {
        h_amt = hB1 + (double)c / (hRes - 1) * planeWidth;
    }
    if (vRes > 1) {
        v_amt = vB2 - (double)r / (vRes - 1) * planeHeight;
    }
    Vector3d pxPoint = ipCpt + (hv * h_amt) + (vv * v_amt);
    Vector3d shoot = pxPoint - focalPoint; shoot.normalize();
    return Ray(pxPoint, shoot);
}

// ******************* Scene *******************

Scene::Scene() {

}

HitRecord Scene::getHit(const Ray &r) const {
    HitRecord hit;
    blockStore->doesHit(r, hit);

    HitRecord cur;
    for (const auto &sphere : spheres) {
        if (sphere.doesHit(r, cur) && (!hit.didHit|| cur.distance < hit.distance) ) {
            hit = cur;
            hit.didHit = true;
        }
    }

    return hit;
}

Material Scene::getMaterial(int blockType, int side, double a, double b) const {
    if (blockType < 0) {
        return sphereMaterials[-blockType-1];
    }

    auto x = blockMaterials.find(blockType);
    if (x == blockMaterials.end())
        return blockMaterials.at(0).getMaterialAt(a,b,side);

    return blockMaterials.at(blockType).getMaterialAt(a,b,side);
}

Vector3d Scene::getSkyBoxColor(const Ray &ray) const {
    if (!skyBoxEnabled) return Vector3d(0,0,0);
    if (!textureSkySphereEnabled) return skyBoxColor;
    return skySphere.getSkyColor(ray);
}

bool SkySphere::intersect(const Ray &ray, Vector3d &hitPoint) const {

    Ray ray1 = ray;
    ray1.o = ray.o + ray.d *1000000;
    ray1.d = -ray.d;

    Vector3d direct = position - ray1.o;
    double directLen = direct.norm();
    double projectLen = direct.dot(ray1.d);
    double dsq = radius * radius - (directLen * directLen - projectLen * projectLen);

//    double epislon = 1e-9;
//    if (fabs(directLen - r) < epislon) {
//        return false;
//    }

    if (dsq > 0) {
        double tval = projectLen - sqrt(dsq);
//        if (tval < 1e-9)
//            return false;

        hitPoint = ray1.o + (projectLen - std::sqrt(dsq)) * ray1.d;
        return true;
    } else {
        return false;
    }
}

Vector3d SkySphere::getSkyColor(const Ray &ray) const {
    Vector3d ipt;

    bool didIntersect = intersect(ray, ipt);
    if (!didIntersect) return Vector3d(0,0,0);

    Vector3d fromCenter =  ipt - position;

    double elev = std::abs(fromCenter[1]) / radius;

    double radii = std::sqrt(1.000000001-elev*elev) / radiiScale;
    double angle = std::atan2(fromCenter[2], fromCenter[0])-1; // +1 to rotate the sky11 tex

    double x = radii * std::cos(angle) + 0.5;
    double y = radii * std::sin(angle) + 0.5;

    return material.getColorAt(x,y);
}


// ******************* Ray Tracing *******************


Vector3d getLightContribution(const Scene &scene, const Vector3d &toEye, const HitRecord &hit, const Material &mat) {

    Vector3d colorRet(0, 0, 0);

    for (const LightSource &light : scene.lights) {
        Vector3d toLightVector = light.position - hit.point;
        double toLightD = toLightVector.norm();
        toLightVector.normalize();

        double toLightDotNormal = toLightVector.dot(hit.normal);
        if (toLightDotNormal < 0) {
            continue;
        }

        Ray toLightRay(hit.point, toLightVector);
        HitRecord hit = scene.getHit(toLightRay);
        if (hit.didHit && hit.distance < toLightD) {
            continue;
        }

        // diffuse lighting
        colorRet = colorRet + entrywiseProduct(light.emittance, mat.kd) * toLightDotNormal;

        // specular lighting
        Vector3d reflect = 2 * toLightVector.dot(hit.normal) * hit.normal - toLightVector;
        double coefficient = toEye.dot(reflect);
        if (coefficient > 0) {
            colorRet = colorRet + entrywiseProduct(light.emittance, mat.ks) * pow(coefficient, mat.ns);
        }

    }

    // ambient light
    colorRet = colorRet + entrywiseProduct(mat.ka, scene.ambientLight);
    return colorRet;
}


Vector3d traceRayHelper(const Scene &scene, const Ray&ray, int depth, int maxDepth) {
    if (depth > maxDepth) {
        return Vector3d(0,0,0);
    }

    HitRecord hit = scene.getHit(ray);
    if (!hit.didHit) {
        return scene.getSkyBoxColor(ray);
    }

    const Material &material = scene.getMaterial(hit.type, hit.side, hit.a, hit.b);

    const Vector3d incomingReversed = -ray.d;
    Vector3d reflectSpecularScaled;
    if (material.kr.norm() > 0) {
        const Vector3d reflectDir = 2 * incomingReversed.dot(hit.normal) * hit.normal - incomingReversed;
        const Ray reflectRay(hit.point, reflectDir);
        const Vector3d reflectColor = traceRayHelper(scene, reflectRay, depth + 1, maxDepth);
        // reflective
        reflectSpecularScaled = entrywiseProduct(material.kr, reflectColor);
    }

    Vector3d lightsColor = getLightContribution(scene, incomingReversed, hit, material);
    return lightsColor + reflectSpecularScaled;
}


Pixel traceRay(const RenderContext &ctx, const Ray &ray) {
    Vector3d colorSample = traceRayHelper(*ctx.scene, ray, 1, ctx.options->maxDepth);
    Vector3d color;

    // convert to 0-255
    for (int i = 0; i < 3; i++) {
        color[i] = std::max(0.0, std::min(255.0, std::round(colorSample[i] * 255.0)));
    }
    return Pixel(color[0], color[1], color[2]);
}

struct Job {
    const RenderContext *ctx;
    int tid;

    Job(const RenderContext *ctx, int tid) :ctx(ctx), tid(tid) {}

    void operator()() {
        int hRes =ctx->options->horizontalResolution;
        int vRes = ctx->options->verticalResolution;

        for (int r = tid; r < vRes; r += ctx->options->threadCount) {
            for (int c = 0; c < hRes; c++) {
                ctx->image->pxAt(r,c) = traceRay(*ctx, ctx->scene->camera->pixelRay(r, c, hRes, vRes));
            }
        }
    }
};

void rt::rayTrace(const RenderContext &ctx) {
    int tc = ctx.options->threadCount;
    std::thread *threads[tc];
    for (int tid = 0; tid < tc; tid++) {
        std::thread *tp = new std::thread(Job(&ctx, tid));
        threads[tid] = tp;
    }
    for (auto thread : threads)
    {
        thread->join();
        delete thread;
    }
}

Material SolidColorMaterial::getMaterialAt(double a, double b) const {
    return mat;
}

Vector3d SolidColorMaterial::getColorAt(double a, double b) const {
    return mat.kd;
}

Material TextureMaterial::getMaterialAt(double a, double b) const {
    return matFromPixel(t->sampleAt(a,b));
}

Vector3d TextureMaterial::getColorAt(double a, double b) const {
    auto pix =t->sampleAt(a,b);
    return Vector3d(pix.r, pix.g, pix.b) / 255;
}

BlockMaterial BlockMaterial::makeUniformBlock(MetaMaterial *mat) {
    BlockMaterial ret;
    for (int i = 0; i < 6; i++) {
        ret.mats[i] = mat;
    }
    return ret;
}

BlockMaterial BlockMaterial::makeTopAndSideBlock(MetaMaterial *top, MetaMaterial *side) {
    BlockMaterial ret;
    ret.mats[0] = top;
    for (int i = 1; i < 6; i++) {
        ret.mats[i] = side;
    }
    return ret;
}

Material BlockMaterial::getMaterialAt(double a, double b, int side) const {
    return mats[side]->getMaterialAt(a,b);
}

Material MetaMaterial::matFromHex(int r, int g, int b) const {
    Material ret;
    const Vector3d scaledValues = Vector3d(
            r/255.0L,
            g/255.0L,
            b/255.0L);

    ret.ka = temp.ambient * scaledValues;
    ret.kd = temp.diffuse * scaledValues;
    ret.ks = temp.specular * scaledValues;
    ret.kr = temp.reflective * scaledValues;
    ret.ns = temp.ns;
    return ret;
}

Material MetaMaterial::matFromPixel(Pixel p) const {
    return matFromHex(p.r,p.g,p.b);
}

Vector3i minVec(Vector3i v1, Vector3i v2) {
    return Vector3i(
            std::min(v1[0],v2[0]),
            std::min(v1[1],v2[1]),
            std::min(v1[2],v2[2]));
}

Vector3i maxVec(Vector3i v1, Vector3i v2) {
    return Vector3i(
            std::max(v1[0],v2[0]),
            std::max(v1[1],v2[1]),
            std::max(v1[2],v2[2]));
}

bool BlockContainer::addBlock(const Block &b) {
    int chunkX  =  getChunkPos(b.position[0], ContainerSize);
    int chunkZ  =  getChunkPos(b.position[2], ContainerSize);

    if (chunkX == x && chunkZ == z){
        min = minVec(min, b.position);
        max = maxVec(max, b.position + Vector3i(1,1,1));
        blocks.push_back(b);
        return true;
    }
    return false;
}

bool BlockContainer::doesHit(const Ray &r, HitRecord &hit) const {
    if (!doesIntersectBox(r)) return false;
    HitRecord cur;
    for (const auto &block : blocks) {
        if (block.doesHit(r, cur) && (!hit.didHit|| cur.distance < hit.distance) ) {
            hit = cur;
            hit.didHit = true;
            hit.type = block.type;
        }
    }
    return hit.didHit;
}

bool BlockContainer::doesIntersectBox(const Ray &r) const {
    double tmin = 1e-9, tmax = INFINITY;

    if (r.d[0] != 0.0) {
        double tx1 = (min[0] - r.o[0])/r.d[0];
        double tx2 = (max[0] - r.o[0])/r.d[0];

        tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
    }

    if (r.d[1] != 0.0) {
        double ty1 = (min[1] - r.o[1])/r.d[1];
        double ty2 = (max[1] - r.o[1])/r.d[1];

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    }

    if (r.d[2] != 0.0) {
        double tz1 = (min[2] - r.o[2])/r.d[2];
        double tz2 = (max[2] - r.o[2])/r.d[2];

        tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));
    }

    return tmax >= tmin;
}

int BlockContainer::getChunkPos(int pos, int size) {
    return (int)std::floor((double) pos / size);
}

MaterialPart::MaterialPart(double ambient, double diffuse, double specular, double reflective, double ns) : ambient(
        ambient), diffuse(diffuse), specular(specular), reflective(reflective), ns(ns) {}

bool Sphere::doesHit(const Ray &ray, HitRecord &hit) const {

    Vector3d direct =  p - ray.o;
    double directLen = direct.norm();
    double projectLen = direct.dot(ray.d);
    double dsq = r * r - (directLen * directLen - projectLen * projectLen);

    if (dsq > 0) {
        double tval = projectLen - sqrt(dsq);
        if (tval < 1e-9)
            return false;

        hit.point = ray.o + (projectLen - std::sqrt(dsq)) * ray.d;
        hit.distance = (hit.point - ray.o).norm();
        hit.normal = hit.point - p; hit.normal.normalize();
        hit.type = type;

        return true;
    } else {
        return false;
    }
}
