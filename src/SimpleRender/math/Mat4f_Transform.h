#pragma once
#include "Vector4f.h"
#include "Mat3f.h"

// (row, colomn)
class Mat4f {
public:
	float data[4][4];
	inline float &at(int i, int j) {
		// i, j within [0,3]
		return this->data[i][j];
	}
public:
	Mat4f();
	Mat4f(float, float, float, float, 
		float, float, float, float, 
		float, float, float, float,
		float, float, float, float);
	Mat4f(const Mat4f &b);
	Mat4f(const Mat3f &b);
	~Mat4f();

public:
	void load_identity();
	void load_zero();
	void operator = (const Mat4f &b);
	void operator = (const Mat3f &b);

public:
	friend const Mat4f operator + (const Mat4f &a, const Mat4f &b);
	friend const Mat4f operator - (const Mat4f &a, const Mat4f &b);
	friend const Mat4f scalar_by(const Mat4f &a, const float k);
	friend const Mat4f operator * (const Mat4f &a, const float k);
	friend const Mat4f operator * (const Mat4f &a, const Mat4f &b);
	friend const Vector4f operator * (const Mat4f &a, const Vector4f &b);
	friend const Mat4f transpose(const Mat4f &a);

public:
	// transform & transform matrices
	friend const Mat4f translation_matrix(const Vector3f &t);
	friend const Mat4f rotate_matrix(const Vector3f &axis, float degree);
	friend const Mat4f rotate_by_x_matrix(float degree);
	friend const Mat4f rotate_by_y_matrix(float degree);
	friend const Mat4f rotate_by_z_matrix(float degree);
	friend const Mat4f scale_matrix(const Vector3f &s);

	void translation(const Vector3f &t);
	void rotate(const Vector3f &axis, float degree);
	void rotate_by_x(float degree);
	void rotate_by_y(float degree);
	void rotate_by_z(float degree);
	void scale(const Vector3f &s);
	void tranform(const Mat4f &b);

public:
	// camera, projection, viewport matrices
	void load_look_at(const Vector3f &eye, const Vector3f &up, const Vector3f &center);
	void load_perspective(float fovy, float aspect, float near, float far);
	void load_ortho(float left, float right, float bottom, float top, float near, float far);
	void load_viewport(int left, int right, int width, int height);
};

// point direct tranform
Vector3f point_translation(const Vector3f &point, const Vector3f &t);
Vector3f point_rotate(const Vector3f &point, const Vector3f &axis, float degree);
Vector3f point_rotate_by_x(const Vector3f &point, float degree);
Vector3f point_rotate_by_y(const Vector3f &point, float degree);
Vector3f point_rotate_by_z(const Vector3f &point, float degree);
Vector3f point_scale(const Vector3f &point, const Vector3f &s);