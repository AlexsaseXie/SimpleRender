#include "SimpleShader.h"
#include "../pipeline/Texture2D.h"

SimpleShader::SimpleShader() {
	m_unit = nullptr;
	m_model_to_world_matrix.load_identity();
	m_model_to_world_inverse_transposed_matrix.load_identity();
	m_world_to_view_matrix.load_identity();
	m_projection_matrix.load_identity();
}

VertexOut SimpleShader::vertex_shader(const VertexIn &in) {
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

Vector4f SimpleShader::fragment_shader(const VertexOut &in) {
	Vector4f litColor = in.color;
	if (m_unit!=nullptr) {
		litColor = m_unit->sample(in.texture_coord);
	}
	return litColor;
}