#pragma once
#include <QVector>
#include "Vector3f.h"

class Vector4f {
public:
	float data[4];
	inline float &x() { return data[0]; }
	inline float &y() { return data[1]; }
	inline float &z() { return data[2]; }
	inline float &w() { return data[3]; }
public:
	Vector4f();
	Vector4f(float, float, float, float);
	Vector4f(const Vector4f &b);
	Vector4f(const Vector3f &b);
	~Vector4f();

public:
	// assign and operations
	void operator = (const Vector4f &b);
	void operator = (const Vector3f &b);
	float& operator [](int i);
	void divide_by_w_inplace();

public:
	friend const Vector4f operator + (const Vector4f &a, const Vector4f &b);
	friend const Vector4f operator - (const Vector4f &a, const Vector4f &b);
	friend const Vector3f divide_by_w(const Vector4f &a);
	friend const Vector4f scalar_by(const Vector4f &a, const float k);
	friend const Vector4f operator * (const Vector4f &a, const float k);
	friend const Vector4f normalize(const Vector4f &a);
	friend const Vector4f linear_p(const Vector4f &a, const Vector4f &b, double weight);
	friend const Vector4f operator * (const Vector4f &a, const Vector4f &b);
};