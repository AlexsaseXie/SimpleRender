#pragma once
#include "../math/Vector3f.h"

class Camera {
public:
	static Vector3f eye;
	static Vector3f center;
	static Vector3f up;
public:
	Camera() { 
		eye = Vector3f(0, 0, -1);
		center = Vector3f(0, 0, 0);
		up = Vector3f(0, 1, 0);
	}
};

class UnboundedCamera : public Camera {
public:
	void rotate_left(float degree);
	void rotate_right(float degree);
	void rotate_up(float degree);
	void rotate_down(float degree);
	void view_rotate(float degree);

public:
	void move_left(float dr);
	void move_right(float dr);
	void move_up(float dr);
	void move_down(float dr);
	void move_near(float dr);
	void move_away(float dr);
};

// to be done
class FirstPersonCamera : public Camera {

};

class ThirdPersonCamera : public Camera {

};
