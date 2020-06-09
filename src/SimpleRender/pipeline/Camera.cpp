#include "Camera.h"
#include "../math/Mat4f_Transform.h"

// static variable initialization
Vector3f Camera::eye = Vector3f(0, 0, -1);
Vector3f Camera::center = Vector3f(0, 0, 0);
Vector3f Camera::up = Vector3f(0, 1, 0);

void UnboundedCamera::rotate_left(float degree) {
	eye = eye - center;
	eye = point_rotate(eye, up, degree);
	eye = eye + center;
}

void UnboundedCamera::rotate_right(float degree) {
	rotate_left(-degree);
}

void UnboundedCamera::rotate_up(float degree) {
	eye = eye - center;
	Vector3f x = cross(up, eye);
	x = normalize(x);

	eye = point_rotate(eye, x, degree);
	up = cross(eye, x);
	up = normalize(up);

	eye = eye + center;
}

void UnboundedCamera::rotate_down(float degree) {
	rotate_up(-degree);
}

void UnboundedCamera::view_rotate(float degree) {
	Vector3f axis = center - eye;
	up = point_rotate(up, axis, degree);
}

void UnboundedCamera::move_near(float dr) {
	Vector3f p = normalize(eye - center);

	eye = eye - p * dr;
	center = center - p * dr;
}

void UnboundedCamera::move_away(float dr) {
	move_near(-dr);
}

void UnboundedCamera::move_left(float dr) {
	Vector3f left = cross(up, center - eye);
	left = normalize(left);
	
	eye = eye + left * dr;
	center = center + left * dr;
}

void UnboundedCamera::move_right(float dr) {
	move_left(-dr);
}

void UnboundedCamera::move_up(float dr) {
	up = normalize(up);
	eye = eye + up * dr;
	center = center + up * dr;
}

void UnboundedCamera::move_down(float dr) {
	move_up(-dr);
}