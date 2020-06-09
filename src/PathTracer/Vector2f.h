#pragma once

class Vector2f {
public:
	float data[2];
	inline float &x() { return data[0]; }
	inline float &y() { return data[1]; }
public:
	Vector2f();
	Vector2f(float, float);
	~Vector2f();

public:
	// assign and operations
	void operator = (const Vector2f &b);
	float& operator [](int i);

	friend const Vector2f operator + (const Vector2f &a, const Vector2f &b);
	friend const Vector2f operator - (const Vector2f &a, const Vector2f &b);
	friend const Vector2f scalar_by(const Vector2f &a, float k);
	friend const Vector2f operator * (const Vector2f &a, float k);
	friend const Vector2f linear_p(const Vector2f &a, const Vector2f &b, double weight);
};