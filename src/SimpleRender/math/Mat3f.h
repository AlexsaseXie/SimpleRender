#pragma once
#include "Vector3f.h"

// (row, colomn)
class Mat3f {
public:
	float data[3][3];
	inline float &at(int i, int j) {
		// i, j within [0,2]
		return this->data[i][j];
	}
public:
	Mat3f();
	Mat3f(float,float,float,float,float,float,float,float,float);
	~Mat3f();

public:
	void load_identity();	
	void operator = (const Mat3f &b);

public:
	friend const Mat3f operator + (const Mat3f &a, const Mat3f &b);
	friend const Mat3f operator - (const Mat3f &a, const Mat3f &b);
	friend const Mat3f scalar_by (const Mat3f &a, const float k);
	friend const Mat3f operator * (const Mat3f &a, const float k);
	friend const Mat3f operator * (const Mat3f &a, const Mat3f &b);
	friend const Vector3f operator * (const Mat3f &a, const Vector3f &b);
};