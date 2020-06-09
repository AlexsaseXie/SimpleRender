#include "Utils.h"

float redians(float degree) {
	return degree / 180.0 * M_PI;
}

float angles(float redian) {
	return redian / M_PI * 180.0;
}

#define FLOAT_EPS 1e-5;
bool equal(float a, float b) {
	return fabs(a - b) < FLOAT_EPS;
}