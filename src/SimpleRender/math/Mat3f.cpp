#include "Mat3f.h"

Mat3f::Mat3f() {
	this->load_identity();
}

Mat3f::Mat3f(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
	this->data[0][0] = a;
	this->data[0][1] = b;
	this->data[0][2] = c;
	this->data[1][0] = d;
	this->data[1][1] = e;
	this->data[1][2] = f;
	this->data[2][0] = g;
	this->data[2][1] = h;
	this->data[2][2] = i;
}	

Mat3f::~Mat3f() {

}

void Mat3f::operator = (const Mat3f &b) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			this->data[i][j] = b.data[i][j];
		}
	}
}

void Mat3f::load_identity() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			this->data[i][j] = i == j ? 1 : 0;
		}
	}
}

const Mat3f operator + (const Mat3f &a, const Mat3f &b) {
	Mat3f result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = a.data[i][j] + b.data[i][j];
		}
	}
	return result;
}

const Mat3f operator - (const Mat3f &a, const Mat3f &b) {
	Mat3f result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = a.data[i][j] - b.data[i][j];
		}
	}
	return result;
}

const Mat3f scalar_by(const Mat3f &a, const float k) {
	Mat3f result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = a.data[i][j] * k;
		}
	}
	return result;
}

const Mat3f operator *(const Mat3f &a, const float k) {
	return scalar_by(a, k);
}

const Mat3f operator * (const Mat3f &a, const Mat3f &b) {
	Mat3f result;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = 0;
			for (int k = 0; k < 3; k++) {
				result.data[i][j] += a.data[i][k] * b.data[k][j];
			}
		}
	}
	return result;
}

const Vector3f operator * (const Mat3f &a, const Vector3f &b) {
	Vector3f result;
	for (int i = 0; i < 3; i++) {
		result.data[i] = 0;
		for (int k = 0; k < 3; k++) {
			result.data[i] += a.data[i][k] * b.data[k];
		}
	}
	return result;
}

