//
// Created by Keegan Millard on 2019-12-05.
//

#include "rt.h"

#include <cmath>
#include <thread>
#include <random>

constexpr double MATH_PI = 3.141592653589793238462643383279502884;

using namespace rt;

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

Vector3d entrywiseProduct(const Vector3d &v1, const Vector3d &v2) {
    return Vector3d(
            v1[0] * v2[0],
            v1[1] * v2[1],
            v1[2] * v2[2]);
}

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

Ray::Ray(const Vector3d &o, const Vector3d &d) : o(o), d(d) {}

// ******************* Materials *******************

Material *Material::makeFromGltfMaterial(const tinygltf::Material &m) {
    const auto pbr = m.pbrMetallicRoughness;
    PbrMaterial *ret = new PbrMaterial;

    ret -> alpha = pbr.baseColorFactor[3];
    ret->baseColor = Vector3d(
            pbr.baseColorFactor[0],
            pbr.baseColorFactor[1],
            pbr.baseColorFactor[2]);
    ret->doubleSided = m.doubleSided;
    ret->name = m.name;
    ret->metallicFactor = pbr.metallicFactor;
    ret->roughnessFactor = pbr.roughnessFactor;

    return ret;
}

Vector3d lerp(const Vector3d &v1, const Vector3d &v2, double a) {
    return Vector3d(
            (v2[0]-v1[0])*a + v1[0],
            (v2[1]-v1[1])*a + v1[1],
            (v2[2]-v1[2])*a + v1[2]);

}


Vector3d specularReflection(const Material &m, )


std::pair<Vector3d, Vector3d> PbrMaterial::getBRDF(const Vector3d &toEye, const Vector3d &toLight,
                                                          const Vector3d &normal) const {
    const Vector3d dielectricSpecular = Vector3d(0.04, 0.04, 0.04);
    const Vector3d black = Vector3d(0, 0, 0);
    const Vector3d one = Vector3d(1,1,1);

    const Vector3d cdiff = lerp(baseColor *(1-dielectricSpecular[0]), black, metallicFactor);
    const Vector3d F0 = lerp(dielectricSpecular, baseColor, metallicFactor);
    const double a = roughnessFactor * roughnessFactor;

    const Vector3d V = toEye;
    const Vector3d L = toLight;
    const Vector3d N = normal;
    const Vector3d H = (L + V) / (L + V).norm();

    const Vector3d F = F0 + (one - F0) * pow(1 - V.dot(H), 5);
    const double G = 0.5 / ( N.dot(L) * sqrt(N.dot(V)*N.dot(V)*(1-a*a)+a*a) + N.dot(V) * sqrt(  N.dot(L)*N.dot(L)*(1-a*a) + a*a ) );
    const double D = a*a / ( MATH_PI * ( N.dot(H)*N.dot(H) * (a*a - 1) + 1) * ( N.dot(H)*N.dot(H) * (a*a - 1) + 1));

    const Vector3d diffuse = cdiff / MATH_PI;

    const Vector3d fDiffuse = entrywiseProduct(one-F, diffuse);
    const Vector3d fSpecular = F * G * D / (4 * N.dot(L) * N.dot(V));

    return std::make_pair(fDiffuse,fSpecular);
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

    PbrMaterial *m0= new PbrMaterial;
    m0->baseColor = Vector3d(0.9, 0.8, 0.7);
    m0->metallicFactor = 1.0;
    m0->roughnessFactor = 0.0;
    m0->alpha = 0;

    PbrMaterial *m1= new PbrMaterial;
    m1->baseColor = Vector3d(0.9, 0.8, 0.7);
    m1->metallicFactor = 1.0;
    m1->roughnessFactor = 0.0;
    m1->alpha = 0;

    PbrMaterial *m2= new PbrMaterial;
    m2->baseColor = Vector3d(0.9, 0.9, 0.9);
    m2->metallicFactor = 1.0;
    m2->roughnessFactor = 0.0;
    m2->alpha = 0;

    PbrMaterial *m3= new PbrMaterial;
    m3->baseColor = Vector3d(0.3, 0.9, 0.3);
    m3->metallicFactor = 1.0;
    m3->roughnessFactor = 0.0;
    m3->alpha = 0;

    PbrMaterial *m4= new PbrMaterial;
    m4->baseColor = Vector3d(0.9, 0.3, 0.3);
    m4->metallicFactor = 1.0;
    m4->roughnessFactor = 0.0;
    m4->alpha = 0;

    PbrMaterial *m5= new PbrMaterial;
    m5->baseColor = Vector3d(0.3, 0.3, 0.9);
    m5->metallicFactor = 1.0;
    m5->roughnessFactor = 0.0;
    m5->alpha = 0;

//    PbrMaterial *m2= new PbrMaterial;
//    m2->baseColor = Vector3d(0.6, 0.9, 0.9);
//    m2->metallicFactor = 1.0;
//    m2->roughnessFactor = 0.0;
//    m2->alpha = 0;


//    Sphere *s1 = new Sphere;
//    s1->point = Vector3d(0,59.8,-20);
//    s1->radius = 50;
//    s1->matIdx = 0;

    Sphere *s2 = new Sphere;
    s2->point = Vector3d(5,-7,-23);
    s2->radius = 3;
    s2->matIdx = 5;

    Sphere *s3 = new Sphere;
    s3->point = Vector3d(-1010,0,-10);
    s3->radius = 1000;
    s3->matIdx = 3;

    Sphere *s4 = new Sphere;
    s4->point = Vector3d(1010,0,-20);
    s4->radius = 1000;
    s4->matIdx = 4;

    Sphere *s5 = new Sphere;
    s5->point = Vector3d(0,-1010,-20);
    s5->radius = 1000;
    s5->matIdx = 2;

    Sphere *s6 = new Sphere;
    s6->point = Vector3d(0,0,-1030);
    s6->radius = 1000;
    s6->matIdx = 2;

    Sphere *s7 = new Sphere;
    s7->point = Vector3d(0,1010,-20);
    s7->radius = 1000;
    s7->matIdx = 2;

    Sphere *s8 = new Sphere;
    s8->point = Vector3d(0,0,1010);
    s8->radius = 1000;
    s8->matIdx = 2;

    Sphere *s9 = new Sphere;
    s9->point = Vector3d(-5,-7, -27);
    s9->radius = 3;
    s9->matIdx = 1;



    LightSource light;
    light.position = Vector3d(0,10,-20);
    light.emittance = Vector3d(1,1,1);

    materials.push_back(std::unique_ptr<Material>(m0));
    materials.push_back(std::unique_ptr<Material>(m1));
    materials.push_back(std::unique_ptr<Material>(m2));
    materials.push_back(std::unique_ptr<Material>(m3));
    materials.push_back(std::unique_ptr<Material>(m4));
    materials.push_back(std::unique_ptr<Material>(m5));


//    objects.push_back(std::unique_ptr<Object>(s1));
    objects.push_back(std::unique_ptr<Object>(s2));
    objects.push_back(std::unique_ptr<Object>(s3));
    objects.push_back(std::unique_ptr<Object>(s4));
    objects.push_back(std::unique_ptr<Object>(s5));
    objects.push_back(std::unique_ptr<Object>(s6));
    objects.push_back(std::unique_ptr<Object>(s7));
    objects.push_back(std::unique_ptr<Object>(s8));
    objects.push_back(std::unique_ptr<Object>(s9));


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

Vector3d traceRayHelper(const Scene &scene, const Ray&ray, int depth, int maxDepth) {
    if (depth > maxDepth) {
        return Vector3d(0,0,0);
    }

    HitRecord hit = scene.findHit(ray);
    if (!hit.didHit) {
        return Vector3d(0,0,0);
    }

    const Material &material = scene.getMatAtIdx(hit.matIdx);
    const Vector3d incomingReversed = -ray.d;

    const Vector3d reflectDir = 2 * incomingReversed.dot(hit.normal) * hit.normal - incomingReversed;
    const Ray reflectRay(hit.point, reflectDir);


    Vector3d colorOut(0,0,0);

    Vector3d reflectColor = traceRayHelper(scene, reflectRay, depth+1, maxDepth);
    const auto x =  material.getBRDF(incomingReversed, reflectDir, hit.normal);
    colorOut = colorOut + entrywiseProduct(x.second, reflectColor);


    for (const LightSource &light : scene.lights) {

        Vector3d toLightVector = light.position - hit.point;
        double toLightD = toLightVector.norm();
        toLightVector.normalize();

        double toLightDotNormal = toLightVector.dot(hit.normal);
        if (toLightDotNormal < 0) {
            continue;
        }

        HitRecord lightHit = scene.findHit(ray);
        if (lightHit.didHit && lightHit.distance < toLightD) {
            continue;
        }

        const auto brdf =  material.getBRDF(incomingReversed, reflectDir, hit.normal);
        const auto diffuseColor =  entrywiseProduct(brdf.first, light.emittance);
        const auto specularColor = entrywiseProduct(brdf.second, light.emittance);

        colorOut = colorOut + diffuseColor + specularColor;
    }

    return colorOut;
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

int rt::test() {
    Vector3d v1(-1,-2,-3);
    Vector3d v2(-5,-6,-7);
    double a = 0.75;

    Vector3d result = lerp(v2,v1,a);

    std::cout << result << std::endl;
}


