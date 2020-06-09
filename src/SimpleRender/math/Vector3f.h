#pragma once
#include <QVector>

class Vector3f {
public:
	float data[3];
	inline float &x() { return data[0]; }
	inline float &y() { return data[1]; }
	inline float &z() { return data[2]; }
public:
	Vector3f();
	Vector3f(float, float, float);
	~Vector3f();

public:
	// assign and operations
	void operator = (const Vector3f &b);
	float& operator [](int i);

public:
	friend const Vector3f operator + (const Vector3f &a, const Vector3f &b);
	friend const Vector3f operator - (const Vector3f &a, const Vector3f &b);
	friend const Vector3f cross(const Vector3f &a, const Vector3f &b);
	friend const float dot(const Vector3f &a, const Vector3f &b);
	friend const Vector3f scalar_by(const Vector3f &a, const float k);
	friend const Vector3f operator * (const Vector3f &a, const float k);
	friend const Vector3f normalize(const Vector3f &a);
	friend const Vector3f linear_p(const Vector3f &a, const Vector3f &b, double weight);
	friend const Vector3f operator * (const Vector3f &a, const Vector3f &b);
	friend const float mol_length(const Vector3f &a);
};