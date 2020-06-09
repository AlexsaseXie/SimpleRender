#include "Vector2f.h"

Vector2f::Vector2f() {
	for (int i = 0; i < 2; i++) {
		this->data[i] = 0;
	}
}

Vector2f::Vector2f(float x, float y) {
	this->data[0] = x;
	this->data[1] = y;
}

Vector2f::~Vector2f() {

}

float& Vector2f::operator [](int i) {
	// i must within [0,1]
	return this->data[i];
}

void Vector2f::operator = (const Vector2f &b) {
	for (int i = 0; i < 2; i++) {
		this->data[i] = b.data[i];
	}
}

const Vector2f operator + (const Vector2f &a, const Vector2f &b) {
	return Vector2f(a.data[0] + b.data[0], a.data[1] + b.data[1]);
}

const Vector2f operator - (const Vector2f &a, const Vector2f &b) {
	return Vector2f(a.data[0] - b.data[0], a.data[1] - b.data[1]);
}

const Vector2f scalar_by(const Vector2f &a, float k) {
	return Vector2f(a.data[0] * k, a.data[1] * k);
}

const Vector2f operator * (const Vector2f &a, float k) {
	return scalar_by(a, k);
}

const Vector2f linear_p(const Vector2f &a, const Vector2f &b, double weight) {
	return a * (1.0 - weight) + b * weight;
}