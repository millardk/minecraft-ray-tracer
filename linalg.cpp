//
// Created by Keegan Millard on 2019-10-10.
//

#include "linalg.h"

#include <cmath>
#include <string>
#include <iostream>

// ***************************** Vector3d *****************************

rt::Vector3d::Vector3d() {
    dat[0] = 0;
    dat[1] = 0;
    dat[2] = 0;
}

rt::Vector3d::Vector3d(double a, double b, double c) {
    dat[0] = a;
    dat[1] = b;
    dat[2] = c;
}

rt::Vector3d::Vector3d(const rt::Vector3d &v) {
    dat[0] = v.dat[0];
    dat[1] = v.dat[1];
    dat[2] = v.dat[2];
}

rt::Vector3d &rt::Vector3d::operator=(const rt::Vector3d &v) {
    dat[0] = v.dat[0];
    dat[1] = v.dat[1];
    dat[2] = v.dat[2];
    return *this;
}

double rt::Vector3d::dot(const rt::Vector3d &v) const {
    return dat[0]*v.dat[0] + dat[1]*v.dat[1] + dat[2]*v.dat[2];
}

rt::Vector3d rt::Vector3d::cross(const rt::Vector3d &v) const {
    double a = dat[1]*v.dat[2] - dat[2]*v.dat[1];
    double b = - (dat[0]*v.dat[2]-dat[2]*v.dat[0]);
    double c = dat[0]*v.dat[1] - dat[1]*v.dat[0];

    return rt::Vector3d(a, b, c);
}

double rt::Vector3d::norm() const {
    return std::sqrt(dat[0]*dat[0] + dat[1]*dat[1] + dat[2]*dat[2]);
}

double &rt::Vector3d::operator[](int x) {
    return dat[x];
}

double rt::Vector3d::operator[](int x) const {
    return dat[x];
}

rt::Vector3d &rt::Vector3d::operator*=(double s) {
    dat[0] *= s;
    dat[1] *= s;
    dat[2] *= s;
    return *this;
}

rt::Vector3d rt::Vector3d::operator/(double s) const {
    return *this * (1/s);
}

void rt::Vector3d::normalize() {
    this->operator=(this->operator/(this->norm()));
}

rt::Vector3d rt::Vector3d::operator+(const Vector3d &v) const {
    return rt::Vector3d(dat[0]+v.dat[0],dat[1]+v.dat[1],dat[2]+v.dat[2]);
}

rt::Vector3d rt::Vector3d::operator-(const Vector3d &v) const {
    return rt::Vector3d(dat[0]-v.dat[0],dat[1]-v.dat[1],dat[2]-v.dat[2]);
}

rt::Vector3d rt::Vector3d::operator-() const {
    return rt::Vector3d(-dat[0],-dat[1],-dat[2]);
}

rt::Vector3d rt::operator*(double s, const rt::Vector3d &v) {
    return Vector3d(s*v.dat[0], s*v.dat[1], s*v.dat[2]);
}

rt::Vector3d rt::operator*(const rt::Vector3d &v, double s) {
    return s * v;
}

std::ostream& rt::operator<<(std::ostream &o, const rt::Vector3d &v) {
    o << v.to_string();
    return o;
}

bool rt::operator==(const rt::Vector3d &lhs, const rt::Vector3d &rhs) {
    return lhs[0] == rhs[0]
        && lhs[1] == rhs[1]
        && lhs[2] == rhs[2];
}

std::string rt::Vector3d::to_string() const {
    return std::to_string(dat[0]) + " "
           + std::to_string(dat[1]) + " "
           + std::to_string(dat[2]);
}

// ***************************** Vector4d *****************************

rt::Vector4d::Vector4d() {
    dat[0] = 0;
    dat[1] = 0;
    dat[2] = 0;
    dat[3] = 0;
}

rt::Vector4d::Vector4d(double a, double b, double c, double d) {
    this->dat[0] = a;
    this->dat[1] = b;
    this->dat[2] = c;
    this->dat[3] = d;
}

double rt::Vector4d::dot(const Vector4d &v) const {
    return dat[0]*v.dat[0] + dat[1]*v.dat[1] + dat[2]*v.dat[2] + dat[3]*v.dat[3];
}

double &rt::Vector4d::operator[](int x) {
    return dat[x];
}

double rt::Vector4d::operator[](int x) const {
    return dat[x];
}

std::string rt::Vector4d::to_string() const {
    return std::to_string(dat[0]) + " "
           + std::to_string(dat[1]) + " "
           + std::to_string(dat[2]) + " "
           + std::to_string(dat[3]);
}

rt::Vector4d rt::operator*(const rt::Vector4d &v, double s) {
    return rt::Vector4d(s*v.dat[0],s*v.dat[1],s*v.dat[2],s*v.dat[3]);
}

rt::Vector4d rt::operator*(double s, const rt::Vector4d &v) {
    return s * v;
}

// ***************************** Matrix4_4d *****************************

rt::Matrix4_4d::Matrix4_4d() {
    for (int i = 0; i < 4; i++) {
        for (int x = 0; x < 4; x++) {
            dat[i][x] = 0;
        }
    }
}

rt::Matrix4_4d::Matrix4_4d(std::initializer_list<std::initializer_list<double>> l ) {
    int r = 0;
    for (auto &sl : l) {
        int c = 0;
        for (double t : sl) {
            coeffRef(r,c) = t;
            c++;
        }
        r++;
    }
}

rt::Vector4d rt::Matrix4_4d::col_vector(int c) const {
    return Vector4d(dat[0][c],dat[1][c],dat[2][c],dat[3][c]);
}

rt::Vector4d rt::Matrix4_4d::row_vector(int r) const {
    return Vector4d(dat[r][0],dat[r][1],dat[r][2],dat[r][3]);
}

double &rt::Matrix4_4d::coeffRef(int r, int c) {
    return dat[r][c];
}

double rt::Matrix4_4d::coeff(int r, int c) const {
    return dat[r][c];
}

rt::Matrix4_4d rt::Matrix4_4d::transpose() const {
    Matrix4_4d ret;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ret.dat[c][r] = dat[r][c];
        }
    }
    return ret;
}

rt::Vector4d rt::Matrix4_4d::operator*(const rt::Vector4d &v) const {
    return Vector4d(
            this->row_vector(0).dot(v),
            this->row_vector(1).dot(v),
            this->row_vector(2).dot(v),
            this->row_vector(3).dot(v)
            );
}

rt::Matrix4_4d::Matrix4_4d(const rt::Matrix4_4d &m) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            dat[r][c] = m.dat[r][c];
        }
    }
}


rt::Matrix4_4d rt::operator*(double s, const rt::Matrix4_4d &m) {
    Matrix4_4d ret;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ret.dat[r][c] = s * m.dat[r][c];
        }
    }
    return ret;
}

rt::Matrix4_4d rt::operator*(const rt::Matrix4_4d &m, double s) {
    return s * m;
}

rt::Matrix4_4d rt::operator*(const rt::Matrix4_4d &lhs, const rt::Matrix4_4d &rhs) {
    Matrix4_4d ret;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ret.dat[r][c] = lhs.row_vector(r).dot(rhs.col_vector(c));
        }
    }
    return ret;
}

std::string rt::Matrix4_4d::to_string() const {
    std::string s;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 3; c++) {
            s += std::to_string(coeff(r,c)) + "\t";
        }
        s += std::to_string(coeff(r,3)) + "\n";
    }
    return s;
}

std::ostream& rt::operator<<(std::ostream &o, const rt::Matrix4_4d &m) {
    o << m.to_string();
    return o;
}

// ***************************** Vector3i *****************************


rt::Vector3i::Vector3i() {
    dat[0] = 0;
    dat[1] = 0;
    dat[2] = 0;
}

rt::Vector3i::Vector3i(int a, int b, int c) {
    dat[0] = a;
    dat[1] = b;
    dat[2] = c;
}

rt::Vector3i::Vector3i(const Vector3i &v) {
    dat[0] = v.dat[0];
    dat[1] = v.dat[1];
    dat[2] = v.dat[2];
}

int rt::Vector3i::operator[](int x) const {
    return dat[x];
}

int &rt::Vector3i::operator[](int x) {
    return dat[x];
}

rt::Vector3i rt::Vector3i::operator+(const rt::Vector3i &v) const {
    return Vector3i(dat[0]+v.dat[0], dat[1]+v.dat[1], dat[2]+v.dat[2]);
}


std::ostream& rt::operator<<(std::ostream &o, const rt::Vector4d &v) {
    o << v.to_string();
    return o;
}

std::string rt::Vector3i::to_string() const {
    return std::to_string(dat[0]) + " "
    + std::to_string(dat[1]) + " "
    + std::to_string(dat[2]);
}

rt::Ray::Ray(const Vector3d &o, const Vector3d &d) : o(o), d(d) {}
