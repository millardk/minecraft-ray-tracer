//
// Created by Keegan Millard on 2019-10-10.
//

#include "linalg.h"

#include <cmath>
#include <string>
#include <iostream>

// ***************************** Vector3d *****************************

lin::Vector3d::Vector3d() {
    dat[0] = 0;
    dat[1] = 0;
    dat[2] = 0;
}

lin::Vector3d::Vector3d(double a, double b, double c) {
    dat[0] = a;
    dat[1] = b;
    dat[2] = c;
}

lin::Vector3d::Vector3d(const lin::Vector3d &v) {
    dat[0] = v.dat[0];
    dat[1] = v.dat[1];
    dat[2] = v.dat[2];
}

lin::Vector3d &lin::Vector3d::operator=(const lin::Vector3d &v) {
    dat[0] = v.dat[0];
    dat[1] = v.dat[1];
    dat[2] = v.dat[2];
    return *this;
}

double lin::Vector3d::dot(const lin::Vector3d &v) const {
    return dat[0]*v.dat[0] + dat[1]*v.dat[1] + dat[2]*v.dat[2];
}

lin::Vector3d lin::Vector3d::cross(const lin::Vector3d &v) const {
    double a = dat[1]*v.dat[2] - dat[2]*v.dat[1];
    double b = - (dat[0]*v.dat[2]-dat[2]*v.dat[0]);
    double c = dat[0]*v.dat[1] - dat[1]*v.dat[0];

    return lin::Vector3d(a, b, c);
}

double lin::Vector3d::norm() const {
    return std::sqrt(dat[0]*dat[0] + dat[1]*dat[1] + dat[2]*dat[2]);
}

double &lin::Vector3d::operator[](int x) {
    return dat[x];
}

double lin::Vector3d::operator[](int x) const {
    return dat[x];
}

lin::Vector3d &lin::Vector3d::operator*=(double s) {
    dat[0] *= s;
    dat[1] *= s;
    dat[2] *= s;
    return *this;
}

lin::Vector3d lin::Vector3d::operator/(double s) const {
    return *this * (1/s);
}

void lin::Vector3d::normalize() {
    this->operator=(this->operator/(this->norm()));
}

lin::Vector3d lin::Vector3d::operator+(const Vector3d &v) const {
    return lin::Vector3d(dat[0]+v.dat[0],dat[1]+v.dat[1],dat[2]+v.dat[2]);
}

lin::Vector3d lin::Vector3d::operator-(const Vector3d &v) const {
    return lin::Vector3d(dat[0]-v.dat[0],dat[1]-v.dat[1],dat[2]-v.dat[2]);
}

lin::Vector3d lin::Vector3d::operator-() const {
    return lin::Vector3d(-dat[0],-dat[1],-dat[2]);
}

lin::Vector3d lin::operator*(double s, const lin::Vector3d &v) {
    return Vector3d(s*v.dat[0], s*v.dat[1], s*v.dat[2]);
}

lin::Vector3d lin::operator*(const lin::Vector3d &v, double s) {
    return s * v;
}

std::ostream& lin::operator<<(std::ostream &o, const lin::Vector3d &v) {
    o << v.to_string();
    return o;
}

bool lin::operator==(const lin::Vector3d &lhs, const lin::Vector3d &rhs) {
    return lhs[0] == rhs[0]
        && lhs[1] == rhs[1]
        && lhs[2] == rhs[2];
}

std::string lin::Vector3d::to_string() const {
    return std::to_string(dat[0]) + " "
           + std::to_string(dat[1]) + " "
           + std::to_string(dat[2]);
}

// ***************************** Vector4d *****************************

lin::Vector4d::Vector4d() {
    dat[0] = 0;
    dat[1] = 0;
    dat[2] = 0;
    dat[3] = 0;
}

lin::Vector4d::Vector4d(double a, double b, double c, double d) {
    this->dat[0] = a;
    this->dat[1] = b;
    this->dat[2] = c;
    this->dat[3] = d;
}

double lin::Vector4d::dot(const Vector4d &v) const {
    return dat[0]*v.dat[0] + dat[1]*v.dat[1] + dat[2]*v.dat[2] + dat[3]*v.dat[3];
}

double &lin::Vector4d::operator[](int x) {
    return dat[x];
}

double lin::Vector4d::operator[](int x) const {
    return dat[x];
}

std::string lin::Vector4d::to_string() const {
    return std::to_string(dat[0]) + " "
           + std::to_string(dat[1]) + " "
           + std::to_string(dat[2]) + " "
           + std::to_string(dat[3]);
}

lin::Vector4d lin::operator*(const lin::Vector4d &v, double s) {
    return lin::Vector4d(s*v.dat[0],s*v.dat[1],s*v.dat[2],s*v.dat[3]);
}

lin::Vector4d lin::operator*(double s, const lin::Vector4d &v) {
    return s * v;
}

// ***************************** Matrix4_4d *****************************

lin::Matrix4_4d::Matrix4_4d() {
    for (int i = 0; i < 4; i++) {
        for (int x = 0; x < 4; x++) {
            dat[i][x] = 0;
        }
    }
}

lin::Matrix4_4d::Matrix4_4d(std::initializer_list<std::initializer_list<double>> l ) {
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

lin::Vector4d lin::Matrix4_4d::col_vector(int c) const {
    return Vector4d(dat[0][c],dat[1][c],dat[2][c],dat[3][c]);
}

lin::Vector4d lin::Matrix4_4d::row_vector(int r) const {
    return Vector4d(dat[r][0],dat[r][1],dat[r][2],dat[r][3]);
}

double &lin::Matrix4_4d::coeffRef(int r, int c) {
    return dat[r][c];
}

double lin::Matrix4_4d::coeff(int r, int c) const {
    return dat[r][c];
}

lin::Matrix4_4d lin::Matrix4_4d::transpose() const {
    Matrix4_4d ret;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ret.dat[c][r] = dat[r][c];
        }
    }
    return ret;
}

lin::Vector4d lin::Matrix4_4d::operator*(const lin::Vector4d &v) const {
    return Vector4d(
            this->row_vector(0).dot(v),
            this->row_vector(1).dot(v),
            this->row_vector(2).dot(v),
            this->row_vector(3).dot(v)
            );
}

lin::Matrix4_4d::Matrix4_4d(const lin::Matrix4_4d &m) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            dat[r][c] = m.dat[r][c];
        }
    }
}


lin::Matrix4_4d lin::operator*(double s, const lin::Matrix4_4d &m) {
    Matrix4_4d ret;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ret.dat[r][c] = s * m.dat[r][c];
        }
    }
    return ret;
}

lin::Matrix4_4d lin::operator*(const lin::Matrix4_4d &m, double s) {
    return s * m;
}

lin::Matrix4_4d lin::operator*(const lin::Matrix4_4d &lhs, const lin::Matrix4_4d &rhs) {
    Matrix4_4d ret;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ret.dat[r][c] = lhs.row_vector(r).dot(rhs.col_vector(c));
        }
    }
    return ret;
}

std::string lin::Matrix4_4d::to_string() const {
    std::string s;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 3; c++) {
            s += std::to_string(coeff(r,c)) + "\t";
        }
        s += std::to_string(coeff(r,3)) + "\n";
    }
    return s;
}

std::ostream& lin::operator<<(std::ostream &o, const lin::Matrix4_4d &m) {
    o << m.to_string();
    return o;
}

// ***************************** Vector3i *****************************


lin::Vector3i::Vector3i() {
    dat[0] = 0;
    dat[1] = 0;
    dat[2] = 0;
}

lin::Vector3i::Vector3i(int a, int b, int c) {
    dat[0] = a;
    dat[1] = b;
    dat[2] = c;
}

lin::Vector3i::Vector3i(const Vector3i &v) {
    dat[0] = v.dat[0];
    dat[1] = v.dat[1];
    dat[2] = v.dat[2];
}

int lin::Vector3i::operator[](int x) const {
    return dat[x];
}

int &lin::Vector3i::operator[](int x) {
    return dat[x];
}

std::ostream& lin::operator<<(std::ostream &o, const lin::Vector4d &v) {
    o << v.to_string();
    return o;
}

std::string lin::Vector3i::to_string() const {
    return std::to_string(dat[0]) + " "
    + std::to_string(dat[1]) + " "
    + std::to_string(dat[2]);
}

