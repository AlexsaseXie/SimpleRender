#include "Mat4f_Transform.h"
#include <qmath.h>
#include "Utils.h"

Mat4f::Mat4f() {
	this->load_identity();
}

Mat4f::Mat4f(float a, float b, float c, float d, 
	float e, float f, float g, float h, 
	float i, float j, float k, float l,
	float m, float n, float o, float p) {
	this->data[0][0] = a;
	this->data[0][1] = b;
	this->data[0][2] = c;
	this->data[0][3] = d;

	this->data[1][0] = e;
	this->data[1][1] = f;
	this->data[1][2] = g;
	this->data[1][3] = h;

	this->data[2][0] = i;
	this->data[2][1] = j;
	this->data[2][2] = k;
	this->data[2][3] = l;

	this->data[3][0] = m;
	this->data[3][1] = n;
	this->data[3][2] = o;
	this->data[3][3] = p;
}

Mat4f::Mat4f(const Mat4f &b) {
	*this = b;
}

Mat4f::Mat4f(const Mat3f &b) {
	*this = b;
}

Mat4f::~Mat4f() {

}

void Mat4f::load_zero() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			this->data[i][j] = 0;
		}
	}
}

void Mat4f::load_identity() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			this->data[i][j] = i == j ? 1 : 0;
		}
	}
}

void Mat4f::operator = (const Mat4f &b) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			this->data[i][j] = b.data[i][j];
		}
	}
}

void Mat4f::operator = (const Mat3f &b) {
	this->load_identity();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			this->data[i][j] = b.data[i][j];
		}
	}
}

const Mat4f operator + (const Mat4f &a, const Mat4f &b) {
	Mat4f result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.data[i][j] = a.data[i][j] + b.data[i][j];
		}
	}
	return result;
}

const Mat4f operator - (const Mat4f &a, const Mat4f &b) {
	Mat4f result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.data[i][j] = a.data[i][j] - b.data[i][j];
		}
	}
	return result;
}

const Mat4f scalar_by(const Mat4f &a,const float k) {
	Mat4f result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.data[i][j] = a.data[i][j] * k;
		}
	}
	return result;
}

const Mat4f operator *(const Mat4f &a, const float k) {
	return scalar_by(a, k);
}

const Mat4f operator * (const Mat4f &a, const Mat4f &b) {
	Mat4f result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.data[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				result.data[i][j] += a.data[i][k] * b.data[k][j];
			}
		}
	}
	return result;
}

const Vector4f operator * (const Mat4f &a, const Vector4f &b) {
	Vector4f result;
	for (int i = 0; i < 4; i++) {
		result.data[i] = 0;
		for (int k = 0; k < 4; k++) {
			result.data[i] += a.data[i][k] * b.data[k];
		}
	}
	return result;
}

const Mat4f transpose(const Mat4f &a) {
	Mat4f result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.data[i][j] = a.data[j][i];
		}
	}
	return result;
}

// transform & transform matrix
const Mat4f translation_matrix(const Vector3f &t) {
	return Mat4f(
		1, 0, 0, t.data[0],
		0, 1, 0, t.data[1],
		0, 0, 1, t.data[2],
		0, 0, 0, 1
	);
}

const Mat4f rotate_matrix(const Vector3f &axis, float degree) {
	Vector3f _axis = normalize(axis);
	float x = _axis.x();
	float y = _axis.y();
	float z = _axis.z();

	float redian = redians(degree);
	Mat3f I;
	I.load_identity();
	Mat3f A(
		x*x,x*y,x*z,
		x*y,y*y,y*z,
		x*z,y*z,z*z
	);
	Mat3f B(
		0,-z,y,
		z,0,-x,
		-y,x,0
	);

	return Mat4f(I * cos(redian) + A * (1 - cos(redian)) + B * sin(redian));
}

const Mat4f rotate_by_x_matrix(float degree) {
	float redian = redians(degree);
	return Mat4f(
		1, 0, 0, 0,
		0, cos(redian), -sin(redian), 0,
		0, sin(redian), cos(redian), 0,
		0, 0, 0, 1
	);
}

const Mat4f rotate_by_y_matrix(float degree) {
	float redian = redians(degree);
	return Mat4f(
		cos(redian), 0, sin(redian), 0,
		0, 1, 0, 0,
		-sin(redian), 0, cos(redian), 0,
		0, 0, 0, 1
	);
}

const Mat4f rotate_by_z_matrix(float degree) {
	float redian = redians(degree);
	return Mat4f(
		cos(redian), -sin(redian), 0, 0,
		sin(redian), cos(redian), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

const Mat4f scale_matrix(const Vector3f &s) {
	return Mat4f(
		s.data[0], 0, 0, 0,
		0, s.data[1], 0, 0,
		0, 0, s.data[2], 0,
		0, 0, 0, 1
	);
}

void Mat4f::translation(const Vector3f &t) {
	Mat4f T = translation_matrix(t);
	*this = *this * T;
}
	
void Mat4f::rotate(const Vector3f &axis, float degree) {
	Mat4f T = rotate_matrix(axis, degree);
	*this = *this * T;
}

void Mat4f::rotate_by_x(float degree) {
	Mat4f T = rotate_by_x_matrix(degree);
	*this = *this * T;
}

void Mat4f::rotate_by_y(float degree) {
	Mat4f T = rotate_by_y_matrix(degree);
	*this = *this * T;
}

void Mat4f::rotate_by_z(float degree) {
	Mat4f T = rotate_by_z_matrix(degree);
	*this = *this * T;
}

void Mat4f::scale(const Vector3f &s) {
	Mat4f T = scale_matrix(s);
	*this = *this * T;
}

void Mat4f::tranform(const Mat4f &b) {
	*this = *this * b;
}

Vector3f point_translation(const Vector3f &point, const Vector3f &t) {
	return point + t;
}

Vector3f point_rotate(const Vector3f &point, const Vector3f &axis, float degree) {
	Mat4f T = rotate_matrix(axis, degree);
	Vector4f p(point);
	// apply transform
	p = T * p;

	return Vector3f(p.data[0], p.data[1], p.data[2]);
}

Vector3f point_rotate_by_x(const Vector3f &point, float degree) {
	Mat4f T = rotate_by_x_matrix(degree);
	Vector4f p(point);
	// apply transform
	p = T * p;

	return Vector3f(p.data[0], p.data[1], p.data[2]);
}

Vector3f point_rotate_by_y(const Vector3f &point, float degree) {
	Mat4f T = rotate_by_y_matrix(degree);
	Vector4f p(point);
	// apply transform
	p = T * p;

	return Vector3f(p.data[0], p.data[1], p.data[2]);
}

Vector3f point_rotate_by_z(const Vector3f &point, float degree) {
	Mat4f T = rotate_by_z_matrix(degree);
	Vector4f p(point);
	// apply transform
	p = T * p;

	return Vector3f(p.data[0], p.data[1], p.data[2]);
}

Vector3f point_scale(const Vector3f &point, const Vector3f &s) {
	return Vector3f(
		point.data[0] * s.data[0],
		point.data[1] * s.data[1],
		point.data[2] * s.data[2]
	);
}

// camera, projection, viewport matrices
void Mat4f::load_look_at(const Vector3f &eye, const Vector3f &up, const Vector3f &center) {
	this->load_identity();
	Vector3f z_axis = normalize(eye - center);
	Vector3f x_axis = normalize(cross(up, z_axis));
	Vector3f y_axis = normalize(cross(z_axis, x_axis));

	this->data[0][0] = x_axis.x();
	this->data[0][1] = x_axis.y();
	this->data[0][2] = x_axis.z();

	this->data[1][0] = y_axis.x();
	this->data[1][1] = y_axis.y();
	this->data[1][2] = y_axis.z();

	this->data[2][0] = z_axis.x();
	this->data[2][1] = z_axis.y();
	this->data[2][2] = z_axis.z();

	this->data[0][3] = -dot(x_axis, eye);
	this->data[1][3] = -dot(y_axis, eye);
	this->data[2][3] = -dot(z_axis, eye);
}

void Mat4f::load_perspective(float fovy, float aspect, float near, float far) {
	this->load_zero();
	float r_fovy = redians(fovy);
	float tan_half_fovy = tanf(r_fovy * 0.5);

	this->data[0][0] = 1.0 / (aspect * tan_half_fovy);
	this->data[1][1] = 1.0 / tan_half_fovy;
	this->data[2][2] = -(far + near) / (far - near);
	this->data[2][3] = -(2.0 * far * near) / (far - near);
	this->data[3][2] = -1.0;
}

void Mat4f::load_ortho(float left, float right, float bottom, float top, float near, float far) {
	this->load_identity();

	this->data[0][0] = 2.0f / (right - left);
	this->data[1][1] = 2.0f / (top - bottom);
	this->data[2][2] = -2.0f / (far - near);
	this->data[0][3] = -(right + left) / (right - left);
	this->data[1][3] = -(top + bottom) / (top - bottom);
	this->data[2][3] = -(far + near) / (far - near);
}

void Mat4f::load_viewport(int left, int top, int width, int height) {
	this->load_identity();
	this->data[0][0] = (float)(width) / 2.0f;
	this->data[1][1] = -(float)(height) / 2.0f;
	this->data[0][3] = (float)(left) + (float)(width) / 2.0f;
	this->data[1][3] = (float)(top) + (float)(height) / 2.0f;
}