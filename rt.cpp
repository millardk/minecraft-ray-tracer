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
    if (pbr.metallicFactor > pbr.roughnessFactor) {
        ret = new MetallicMaterial;
    } else {
        ret = new DielectricMaterial;
    }

    ret->baseColor = Vector3d(
            pbr.baseColorFactor[0],
            pbr.baseColorFactor[1],
            pbr.baseColorFactor[2]);
    ret -> alpha = pbr.baseColorFactor[3];
    ret->doubleSided = m.doubleSided;
    ret->name = m.name;
    ret->metallicFactor = pbr.metallicFactor;
    ret->roughnessFactor = pbr.roughnessFactor;
    return ret;
}


Vector3d MetallicMaterial::getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) {
    return rt::Vector3d();
}


Vector3d DielectricMaterial::getBRDF(const Vector3d &d1, const Vector3d &d2, const Vector3d &n) {
    return rt::Vector3d();
}

// ******************* Object *******************

HitRecord Mesh::findHit(const Ray &) {
    return HitRecord();
}

HitRecord Sphere::findHit(const Ray &) {
    return HitRecord();
}

// ******************* Scene *******************


HitRecord Scene::findHit(const Ray &r) {
    return HitRecord();
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



// ******************* Ray Tracing *******************

class RandomGenerator {
private:
    std::vector<Vector3d> store;

    Vector3d getRandomNormalVector() {
        thread_local int idx = 0;
        if (idx > store.size()) {
            idx = 0;
        }
        return store[idx];
    }

public:
    RandomGenerator(int seed, int storeSize) {
        std::default_random_engine eng;
        std::uniform_real_distribution<double> urd(-1, 1);

        for (int i = 0; i < storeSize; i++) {
            double a = urd(eng);
            double b = urd(eng);
            double c = urd(eng);
            Vector3d vec(a, b, c);
            vec.normalize();
            store.push_back(vec);
        }
    }

    Vector3d getRandomNormalVectorInHemisphere(const Vector3d v) {
        Vector3d rvec = getRandomNormalVector();
        if (v.dot(rvec) < 0) {
            rvec *= -1;
        }
        return rvec;
    }
};

RandomGenerator randomGenerator = RandomGenerator(0, 1000);

Vector3d traceRayHelper(const Scene &s, int depth, int maxDepth) {


}

Pixel traceRay(const RenderContext &ctx, const Ray &ray) {
    Vector3d color = traceRayHelper(*ctx.scene, 0, ctx.options->maxDepth);

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
