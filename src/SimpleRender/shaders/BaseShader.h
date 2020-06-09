#pragma once
#include "../math/Vector4f.h"
#include "../math/Mat4f_Transform.h"

class VertexIn;
class VertexOut;
class Texture2D;
class Material;
class Light;

class BaseShader {
public:
	BaseShader() {}
	virtual ~BaseShader() = default;

	// Shader stage.
	virtual VertexOut vertex_shader(const VertexIn &in) = 0;
	virtual Vector4f fragment_shader(const VertexOut &in) = 0;

	// Shader setting.
	virtual void bind_texture_unit(Texture2D *unit) = 0;
	virtual void unbind_texture_unit() = 0;
	virtual void set_model_to_world_matrix(const Mat4f &world) = 0;
	virtual void set_model_to_world_inverse_transposed_matrix(const Mat4f &world_i_t) = 0;
	virtual void set_world_to_view_matrix(const Mat4f &view) = 0;
	virtual void set_projection_matrix(const Mat4f &project) = 0;
	virtual void set_material(Material *material) = 0;
	virtual void add_light(Light *light) = 0;
	virtual void clear_lights() = 0;
	virtual void set_eye(const Vector3f eye) = 0;
};