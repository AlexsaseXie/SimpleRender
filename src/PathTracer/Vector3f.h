#pragma once
#include <QVector>

class Vector3f {
public:
	double data[3];
	inline double &x() { return data[0]; }
	inline double &y() { return data[1]; }
	inline double &z() { return data[2]; }
public:
	Vector3f();
	Vector3f(double, double, double);
	~Vector3f();

public:
	// assign and operations
	void operator = (const Vector3f &b);
	double& operator [](int i);

public:
	friend const Vector3f operator + (const Vector3f &a, const Vector3f &b);
	friend const Vector3f operator - (const Vector3f &a, const Vector3f &b);
	friend const Vector3f cross(const Vector3f &a, const Vector3f &b);
	friend const double dot(const Vector3f &a, const Vector3f &b);
	friend const Vector3f scalar_by(const Vector3f &a, const double k);
	friend const Vector3f operator * (const Vector3f &a, const double k);
	friend const Vector3f normalize(const Vector3f &a);
	friend const Vector3f linear_p(const Vector3f &a, const Vector3f &b, double weight);
	friend const Vector3f operator * (const Vector3f &a, const Vector3f &b);
	friend const double mol_length(const Vector3f &a);
};