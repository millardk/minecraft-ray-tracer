//
// Created by Keegan Millard on 2019-12-05.
//

#include "rt.h"

#include <cmath>
#include <thread>
#include <random>

using namespace rt;

Ray::Ray(const Vector3d &o, const Vector3d &d) : o(o), d(d) {}

// ******************* Materials *******************

Material *Material::makeFromGltfMaterial(const tinygltf::Material &m) {
    const auto pbr = m.pbrMetallicRoughness;
    PbrMaterial *ret;

    if (pbr.extras.Has("emissiveFactor")) {
        ret = new LightSource;
        const auto &x = pbr.extras.Get("emissiveFactor");
        ret -> emissiveFactor[0] = x.Get(0).GetNumberAsDouble();
        ret -> emissiveFactor[1] = x.Get(1).GetNumberAsDouble();
        ret -> emissiveFactor[2] = x.Get(2).GetNumberAsDouble();
        return ret;
    }

    if (pbr.metallicFactor > pbr.roughnessFactor) {
        ret = new MetallicMaterial;
    } else {
        ret = new DielectricMaterial;
    }

    ret -> alpha = pbr.baseColorFactor[3];
    ret->baseColor = Vector3d(
            pbr.baseColorFactor[0],
            pbr.baseColorFactor[1],
            pbr.baseColorFactor[2]);
    ret->doubleSided = m.doubleSided;
    ret->name = m.name;
    ret->metallicFactor = pbr.metallicFactor;
    ret->roughnessFactor = pbr.roughnessFactor;

    if (pbr.extras.Has("emissiveFactor")) {
        const auto &x = pbr.extras.Get("emissiveFactor");
        ret -> emissiveFactor[0] = x.Get(0).GetNumberAsDouble();
        ret -> emissiveFactor[1] = x.Get(1).GetNumberAsDouble();
        ret -> emissiveFactor[2] = x.Get(2).GetNumberAsDouble();
    }

    return ret;
}


Vector3d MetallicMaterial::getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const {
    return d1.dot(d2) * baseColor;
}

Vector3d DielectricMaterial::getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const {
    return d1.dot(d2) * baseColor;
}

Vector3d LightSource::getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) const {
    return emissiveFactor;
}

// ******************* Object *******************

Vector3d calculateSurfaceNormal(const Ray &r, const Vector3d &v0, const Vector3d &v1, const Vector3d &v2){
    Vector3d normal = (v1-v0).cross(v2-v1);
    normal.normalize();

    if (normal.dot(r.d) > 0){
        return -normal;
    } else {
        return normal;
    }
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool doesHitSurface(const Ray &r, const Vector3d &v0, const Vector3d &v1, const Vector3d &v2, HitRecord &hit) {
    const double EPSILON = 1e-9;
    Vector3d edge1, edge2, h, s, q;
    double a,f,u,v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = r.d.cross(edge2);
    a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON)
        return false;    // This ray is parallel to this triangle.
    f = 1.0/a;
    s = r.o - v0;
    u = f * s.dot(h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = s.cross(edge1);
    v = f * r.d.dot(q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = f * edge2.dot(q);
    if (t > EPSILON && t < 1/EPSILON) // ray intersection
    {
        hit.point = r.o + (r.d * t);
        hit.distance = t;
//        hit.normal = edge1.cross(edge2); hit.normal.normalize();
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

bool Mesh::doesHit(const Ray &ray, HitRecord &hit) const {
    bool doesHit = false;
    HitRecord cur;
    for (int x = 0; x < primitives.size(); x++) {
        const Primitive &prim = primitives[x];
        const Vector3d &v0 = vertices[prim.vIndicies[0]];
        const Vector3d &v1 = vertices[prim.vIndicies[1]];
        const Vector3d &v2 = vertices[prim.vIndicies[2]];

        if ( doesHitSurface(ray, v0, v1, v2, cur) && (!doesHit || cur.distance < hit.distance) ) {
            doesHit = true;
            hit = cur;
            hit.matIdx = prim.matIdx;
            hit.normal = calculateSurfaceNormal(ray, v0, v1, v2);
        }
    }
    return doesHit;
}

bool Sphere::doesHit(const Ray &ray, HitRecord &hit) const {
    Vector3d direct = point - ray.o;
    double directLen = direct.norm();
    double projectLen = direct.dot(ray.d);
    double dsq = radius * radius - (directLen * directLen - projectLen * projectLen);

//    double epislon = 1e-9;
//    if (fabs(directLen - r) < epislon) {
//        return false;
//    }

    if (dsq > 0) {
        double tval = projectLen - sqrt(dsq);
        if (tval < 1e-9)
            return false;

        hit.point = ray.o + (projectLen - std::sqrt(dsq)) * ray.d;
        hit.distance = (hit.point - ray.o).norm();
        hit.normal = hit.point - point; hit.normal.normalize();
        hit.matIdx = matIdx;
        return true;
    } else {
        return false;
    }
}

// ******************* Camera *******************

void Camera::init() {
    lv = lookPoint - focalPoint; lv.normalize();
    hv = lv.cross(upVector); hv.normalize();
    vv = hv.cross(lv); vv.normalize();
    ipCpt = focalPoint + (lv * imagePlaneDistance);
    planeWidth = abs(hB2 - hB1);
    planeHeight = abs(vB2 - vB1);
}

Ray Camera::pixelRay(int r, int c, int hRes, int vRes) const {
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


Scene::Scene(const tinygltf::Model &m) {
    using namespace tinygltf;
}

Scene::Scene() {
    LightSource *m0 = new LightSource;
    m0->emissiveFactor = Vector3d(1, 1, 1);
    m0->emissiveFactor *= 1000;

    MetallicMaterial *m1= new MetallicMaterial;
    m1->baseColor = Vector3d(0.9, 0.5, 0.3);
    m1->metallicFactor = 1.0;
    m1->roughnessFactor = 0.0;
    m1->alpha = 0;

    Sphere *s1 = new Sphere;
    s1->point = Vector3d(0,0,-20);
    s1->radius = 4;
    s1->matIdx = 1;

    Sphere *s2 = new Sphere;
    s2->point = Vector3d(0,10,-10);
    s2->radius = 1;
    s2->matIdx = 0;

//    Sphere *s3 = new Sphere;
//    s1->point = Vector3d(0,0,-20);
//    s1->radius = 4;
//    s1->matIdx = 1;
//
//    Sphere *s4 = new Sphere;
//    s1->point = Vector3d(0,0,-20);
//    s1->radius = 4;
//    s1->matIdx = 1;
//

    materials.push_back(std::unique_ptr<Material>(m0));
    materials.push_back(std::unique_ptr<Material>(m1));

    objects.push_back(std::unique_ptr<Object>(s1));
    objects.push_back(std::unique_ptr<Object>(s2));


    camera.focalPoint = Vector3d(0,0,0);
    camera.lookPoint = Vector3d(0,0,-20);
    camera.upVector = Vector3d(0,1,0);
    camera.imagePlaneDistance = 2;
    camera.vB1 = -1;
    camera.vB2 = 1;
    camera.hB1 = -1;
    camera.hB2 = 1;
    camera.init();
}

HitRecord Scene::findHit(const Ray &r) const {
    HitRecord hit;
    HitRecord cur;
    for (const auto &x : objects) {
        const Object &object = *x;
        if (object.doesHit(r, cur) && (!hit.didHit|| cur.distance < hit.distance) ) {
            hit = cur;
            hit.didHit = true;
        }
    }
    return hit;
}

const Material& Scene::getMatAtIdx(int matIdx) const {
    return materials[matIdx].operator*();
}

// ******************* Ray Tracing *******************

Vector3d entrywiseProduct(Vector3d &v1, const Vector3d &v2) {
    return Vector3d(
            v1[0] * v2[0],
            v1[1] * v2[1],
            v1[2] * v2[2]);
}

class RandomGenerator {
private:
    std::default_random_engine eng;
    std::uniform_real_distribution<double> urd;

public:
    RandomGenerator(): urd(-1,1){}

    Vector3d getRandomNormalVector() {
        double a = urd(eng);
        double b = urd(eng);
        double c = urd(eng);
        Vector3d vec(a, b, c);
        vec.normalize();
        return vec;
    }

    Vector3d getRandomNormalVectorInHemisphere(const Vector3d v) {
        Vector3d rvec = getRandomNormalVector();
        if (v.dot(rvec) < 0) {
            rvec *= -1;
        }
        return rvec;
    }
};
RandomGenerator randomGenerator = RandomGenerator();

//class RandomGenerator {
//private:
//    std::vector<Vector3d> store;
//
//public:
//    RandomGenerator(int seed, int storeSize) {
//        std::default_random_engine eng;
//        std::uniform_real_distribution<double> urd(-1, 1);
//
//        for (int i = 0; i < storeSize; i++) {
//            double a = urd(eng);
//            double b = urd(eng);
//            double c = urd(eng);
//            Vector3d vec(a, b, c);
//            vec.normalize();
//            store.push_back(vec);
//        }
//    }
//
//    Vector3d getRandomNormalVector() {
//        thread_local int idx = 0;
//        if (idx > store.size()) {
//            idx = 0;
//        }
//        return store[idx++];
//    }
//
//    Vector3d getRandomNormalVectorInHemisphere(const Vector3d v) {
//        Vector3d rvec = getRandomNormalVector();
//        if (v.dot(rvec) < 0) {
//            rvec *= -1;
//        }
//        return rvec;
//    }
//};
//RandomGenerator randomGenerator = RandomGenerator(0,1000);


Vector3d traceRayHelper(const Scene &scene, const Ray&ray, int depth, int maxDepth) {
    if (depth > maxDepth) {
        return Vector3d(0,0,0);
    }

    HitRecord hit = scene.findHit(ray);
    if (!hit.didHit) {
        return Vector3d(0,0,0);
    }

    Vector3d incomingReversed = -ray.d;

//    Vector3d reflectDir = 2 * incomingReversed.dot(hit.normal) * hit.normal - incomingReversed;
    Vector3d reflectDir = randomGenerator.getRandomNormalVectorInHemisphere(hit.normal);

    assert(reflectDir.dot(hit.normal) >= 0);

    const Material &material = scene.getMatAtIdx(hit.matIdx);
    Vector3d brdf = material.getBRDF(incomingReversed, reflectDir, hit.normal);

    if (dynamic_cast<const LightSource*>(&material)) {
        return brdf;
    }



    Ray rayOut(hit.point, reflectDir);
    Vector3d forwardResult = traceRayHelper(scene, rayOut, depth+1, maxDepth);
    Vector3d ret = entrywiseProduct(brdf, forwardResult);

    return ret;
}

Pixel traceRay(const RenderContext &ctx, const Ray &ray) {

    Vector3d agg;
    for (int sample = 0; sample < ctx.options->samplesPerPixel; sample++) {
        Vector3d colorSample = traceRayHelper(*ctx.scene, ray, 1, ctx.options->maxDepth);
        agg = agg + colorSample;
    }
    Vector3d color = agg / ctx.options->samplesPerPixel;

    // convert to 0-255
    for (int i = 0; i < 3; i++) {
        color[i] = std::max(0.0, std::min(255.0, std::round(color[i] * 255.0)));
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
                ctx->image->pxAt(r,c) = traceRay(*ctx, ctx->scene->camera.pixelRay(r, c, hRes, vRes));
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

int rt::test(int count) {
    double ret = 0;
    for (int i = 0; i < count; i++) {
        auto x = randomGenerator.getRandomNormalVector();
        ret += x[0] + x[1] + x[2];
    }
    return ret;
}


