#pragma once
#include "Vector3f.h"

class Ray { 
public:
	Vector3f o, d;
	Ray() { o = Vector3f(0, 0, 0); d = Vector3f(0, 0, 0); }
	Ray(Vector3f o_, Vector3f d_) : o(o_), d(d_) {} 
};
