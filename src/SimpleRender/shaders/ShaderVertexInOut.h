#pragma once
#include "../math/Vector4f.h"
#include "../math/Vector3f.h"
#include "../math/Vector2f.h"

class VertexIn {
public:
	Vector3f pos_model;
	Vector4f color;
	Vector2f texture_coord;
	Vector3f normal_model;
public:
	VertexIn() {}
	VertexIn(const Vector3f in_pos, const Vector4f in_color, const Vector2f in_tex_coor, const Vector3f in_normal) {
		pos_model = in_pos;
		color = in_color;
		texture_coord = in_tex_coor;
		normal_model = in_normal;
	}
};

class VertexOut {
public:
	Vector4f pos_world;
	Vector4f pos_projected;
	Vector4f color;
	Vector2f texture_coord;
	Vector4f normal_world;
	double one_div_z; // z in pos_world
public:
	VertexOut() {}
	VertexOut(const VertexOut &a) {
		pos_world = a.pos_world;
		pos_projected = a.pos_projected;
		color = a.color;
		texture_coord = a.texture_coord;
		normal_world = a.normal_world;
		one_div_z = a.one_div_z;
	}
};


