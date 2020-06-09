#include "Vector3f.h"
#include <qmath.h>

Vector3f::Vector3f() {
	for (int i = 0; i < 3;i++) {
		this->data[i] = 0;
	}
}

Vector3f::Vector3f(double x, double y, double z) {
	this->data[0] = x;
	this->data[1] = y;
	this->data[2] = z;
}

Vector3f::~Vector3f() {

}

double& Vector3f::operator [](int i) {
	// i must within [0,2]
	return this->data[i];
}

void Vector3f::operator = (const Vector3f &b) {
	for (int i = 0; i < 3; i++) {
		this->data[i] = b.data[i];
	}
}

const Vector3f operator + (const Vector3f &a,const Vector3f &b) {
	return Vector3f(a.data[0] + b.data[0], a.data[1] + b.data[1], a.data[2] + b.data[2]);
}

const Vector3f operator - (const Vector3f &a, const Vector3f &b) {
	return Vector3f(a.data[0] - b.data[0], a.data[1] - b.data[1], a.data[2] - b.data[2]);
}

const Vector3f cross(const Vector3f &a, const Vector3f &b) {
	Vector3f result;
	result[0] = a.data[1] * b.data[2] - a.data[2] * b.data[1];
	result[1] = a.data[2] * b.data[0] - a.data[0] * b.data[2];
	result[2] = a.data[0] * b.data[1] - a.data[1] * b.data[0];
	return result;
}

const double dot(const Vector3f &a ,const Vector3f &b) {
	double result = 0;
	for (int i = 0; i < 3; i++) {
		result += a.data[i] * b.data[i];
	}
	return result;
}

const Vector3f scalar_by(const Vector3f &a, const double k) {
	return Vector3f(a.data[0] * k, a.data[1] * k, a.data[2] * k);
}

const Vector3f operator * (const Vector3f &a, const double k) {
	return scalar_by(a, k);
}

const Vector3f normalize(const Vector3f &a) {
	// cannot be zero vector
	double div = sqrt(a.data[0] * a.data[0] + a.data[1] * a.data[1] + a.data[2] * a.data[2]);
	div = 1. / div;
	return scalar_by(a, div);
}

const Vector3f linear_p(const Vector3f &a, const Vector3f &b, double weight) {
	return a * (1.0 - weight) + b * weight;
}

const Vector3f operator * (const Vector3f &a, const Vector3f &b) {
	return Vector3f(a.data[0] * b.data[0], a.data[1] * b.data[1], a.data[2] * b.data[2]);
}

const double mol_length(const Vector3f &a) {
	double r = a.data[0] * a.data[0] + a.data[1] * a.data[1] + a.data[2] * a.data[2];
	return sqrtf(r);
}