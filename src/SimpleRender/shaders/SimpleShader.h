#pragma once
#include "BaseShader.h"
#include "ShaderVertexInOut.h"

class SimpleShader : public BaseShader {
public:
	Mat4f m_model_to_world_matrix;
	Mat4f m_model_to_world_inverse_transposed_matrix;
	Mat4f m_world_to_view_matrix;
	Mat4f m_projection_matrix;
	Texture2D * m_unit;
	
public:
	SimpleShader();
	virtual ~SimpleShader() = default;

	// Shader stage.
	virtual VertexOut vertex_shader(const VertexIn &in);
	virtual Vector4f fragment_shader(const VertexOut &in);

	// Shader setting.
	virtual void bind_texture_unit(Texture2D *unit) { m_unit = unit; }
	virtual void unbind_texture_unit(){ m_unit = nullptr; }
	virtual void set_model_to_world_matrix(const Mat4f &world) { m_model_to_world_matrix = world; }
	virtual void set_model_to_world_inverse_transposed_matrix(const Mat4f &world_i_t) { m_model_to_world_inverse_transposed_matrix = world_i_t; }
	virtual void set_world_to_view_matrix(const Mat4f &view) { m_world_to_view_matrix = view; }
	virtual void set_projection_matrix(const Mat4f &project) { m_projection_matrix = project; }
	virtual void set_material(Material *material) {}
	virtual void add_light(Light *light) {}
	virtual void clear_lights() {}
	virtual void set_eye(const Vector3f eye) {}
};