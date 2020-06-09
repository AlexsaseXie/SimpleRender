#include "PhongShader.h"
#include "../pipeline/Texture2D.h"
#include "../pipeline/Light_Material.h"

PhongShader::PhongShader() {
	m_unit = nullptr;
	m_model_to_world_matrix.load_identity();
	m_model_to_world_inverse_transposed_matrix.load_identity();
	m_world_to_view_matrix.load_identity();
	m_projection_matrix.load_identity();
	m_material = nullptr;
	m_lights.clear();
}

VertexOut PhongShader::vertex_shader(const VertexIn &in) {
	VertexOut result;
	Vector4f tmp = in.pos_model;
	result.pos_world = m_model_to_world_matrix * tmp;
	result.pos_projected = m_projection_matrix * m_world_to_view_matrix * result.pos_world;
	result.color = in.color;
	// normal is correctly calculated by multiplied by model_mat_inverse_transposed
	tmp = in.normal_model;
	result.normal_world = normalize(m_model_to_world_inverse_transposed_matrix * tmp);
	result.texture_coord = in.texture_coord;

	return result;
}

Vector4f PhongShader::fragment_shader(const VertexOut &in) {
	Vector4f litColor = in.color;
	if (m_unit != nullptr) {
		litColor = m_unit->sample(in.texture_coord);
	}

	if (m_lights.size() > 0 && m_material != nullptr) {
		Vector4f in_color = litColor;
		litColor = Vector4f(0.f, 0.f, 0.f, 1.f);
		Vector3f pos_world = Vector3f(in.pos_world.data[0],in.pos_world.data[1], in.pos_world.data[2]);
		Vector3f normal_world = normalize(Vector3f(in.normal_world.data[0], in.normal_world.data[1], in.normal_world.data[2]));
		Vector4f out_color(0,0,0,1);
		for (auto &f : m_lights) {
			f->phong_lighting(*m_material, pos_world, normal_world, m_eye, in_color, out_color);
			litColor = litColor + out_color;
		}

		litColor.data[0] = qBound(litColor.data[0], 0.0f, 1.0f);
		litColor.data[1] = qBound(litColor.data[1], 0.0f, 1.0f);
		litColor.data[2] = qBound(litColor.data[2], 0.0f, 1.0f);
		litColor.data[3] = 1.0f;
	}
	return litColor;
}