//
// Created by Keegan Millard on 2019-10-10.
//

#ifndef CS410_HW2_LINALG_H
#define CS410_HW2_LINALG_H

#include <initializer_list>
#include <string>

namespace rt {

#ifdef USE_EIGEN

#include "Eigen/Dense"

typedef lin::Vector3d Vector3d;
typedef lin::Vector3i Vector3i;
typedef lin::Vector4d Vector4d;
typedef lin::Matrix4_4d Matrix4_4d;

#else

class Vector3d {
public:
    double dat[3];

    Vector3d();
    Vector3d(double a, double b, double c);
    Vector3d(const Vector3d &v);
    Vector3d &operator=(const Vector3d &v);

    void normalize();
    double dot(const Vector3d &v) const;
    Vector3d cross(const Vector3d &v) const;
    double norm() const;

    Vector3d &operator*=(double s);
    double &operator[](int x);
    double operator[](int x) const;
    Vector3d operator/(double s) const;
    Vector3d operator+(const Vector3d &v) const;
    Vector3d operator-(const Vector3d &v) const;
    Vector3d operator-() const;

    std::string to_string() const;
};

class Vector4d {
public:
    double dat[4];

    Vector4d();
    Vector4d(double a, double b, double c, double d);

    double dot(const Vector4d &v) const;
    double &operator[](int x);
    double operator[](int x) const;

    std::string to_string() const;
};

class Matrix4_4d {
public:
    double dat[4][4];

    Matrix4_4d();
    Matrix4_4d(const Matrix4_4d &m);
    Matrix4_4d(std::initializer_list<std::initializer_list<double>> l);

    Vector4d col_vector(int c) const;
    Vector4d row_vector(int c) const;

    double &coeffRef(int r, int c);
    double coeff(int r, int c) const;
    Matrix4_4d transpose() const;
    Vector4d operator*(const Vector4d &v) const;

    std::string to_string() const;
};

class Vector3i {
public:
    int dat[3];

    Vector3i();
    Vector3i(int a, int b, int c);
    Vector3i(const Vector3i &v);
    int operator[](int x) const;
    int &operator[](int x);
    Vector3i operator+(const Vector3i &v) const;

    std::string to_string() const;
};

Matrix4_4d operator*(double s, const Matrix4_4d &m);
Matrix4_4d operator*(const Matrix4_4d &m, double s);
Matrix4_4d operator*(const Matrix4_4d &lhs, const Matrix4_4d &rhs);
Matrix4_4d operator*(const Matrix4_4d &lhs, const Matrix4_4d &rhs);
std::ostream& operator<<(std::ostream &o, const Matrix4_4d &m);

Vector3d operator*(const Vector3d &v, double s);
Vector3d operator*(double s, const Vector3d &v);
std::ostream& operator<<(std::ostream &o, const Vector3d &v);
bool operator==(const Vector3d &lhs, const Vector3d &rhs);

Vector4d operator*(const Vector4d &v, double s);
Vector4d operator*(double s, const Vector4d &v);
std::ostream& operator<<(std::ostream &o, const Vector4d &v);

#endif

struct Ray {
    Vector3d o;
    Vector3d d;

    Ray(const Vector3d &o, const Vector3d &d);
};

}


#endif //CS410_HW2_LINALG_H
