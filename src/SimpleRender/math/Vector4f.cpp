#include "Vector4f.h"
#include <qmath.h>

Vector4f::Vector4f() {
	for (int i = 0; i < 3; i++) {
		this->data[i] = 0;
	}
	this->data[3] = 1;
}

Vector4f::Vector4f(float x, float y, float z, float w) {
	this->data[0] = x;
	this->data[1] = y;
	this->data[2] = z;
	this->data[3] = w;
}

Vector4f::Vector4f(const Vector4f &b) {
	*this = b;
}

Vector4f::Vector4f(const Vector3f &b) {
	*this = b;
}

Vector4f::~Vector4f() {

}

float& Vector4f::operator [](int i) {
	// i must within [0,3]
	return this->data[i];
}

void Vector4f::operator = (const Vector4f &b) {
	for (int i = 0; i < 4; i++) {
		this->data[i] = b.data[i];
	}
}

void Vector4f::operator = (const Vector3f &b) {
	for (int i = 0; i < 3; i++) {
		this->data[i] = b.data[i];
	}
	this->data[3] = 1.0;
}

void Vector4f::divide_by_w_inplace() {
	double div = this->data[3];
	div = 1. / div;
	for (int i = 0; i < 4; i++) {
		this->data[i] = (double)this->data[i] * div;
	}
	this->data[3] = 1.;
}

const Vector4f operator + (const Vector4f &a, const Vector4f &b) {
	return Vector4f(a.data[0] + b.data[0], a.data[1] + b.data[1], a.data[2] + b.data[2], a.data[3] + b.data[3]);
}

const Vector4f operator - (const Vector4f &a, const Vector4f &b) {
	return Vector4f(a.data[0] - b.data[0], a.data[1] - b.data[1], a.data[2] - b.data[2], a.data[3] - b.data[3]);
}

const Vector3f divide_by_w(const Vector4f &a) {
	double div = a.data[3];
	div = 1. / div;
	return Vector3f(a.data[0] * div, a.data[1] * div, a.data[2] * div);
}

const Vector4f scalar_by(const Vector4f &a, const float k) {
	Vector4f result;
	for (int i = 0; i < 4; i++) {
		result.data[i] = a.data[i] * k;
	}
	return result;
}

const Vector4f operator * (const Vector4f &a, const float k) {
	return scalar_by(a, k);
}

const Vector4f normalize(const Vector4f &a_) {
	Vector3f a = divide_by_w(a_);
	a = normalize(a);
	return Vector4f(a.data[0], a.data[1], a.data[2], 1.0f);
}

const Vector4f linear_p(const Vector4f &a, const Vector4f &b, double weight) {
	return a * (1.0 - weight) + b * weight;
}

const Vector4f operator * (const Vector4f &a, const Vector4f &b) {
	return Vector4f(a.data[0] * b.data[0], a.data[1] * b.data[1], a.data[2] * b.data[2], a.data[3] * b.data[3]);
}