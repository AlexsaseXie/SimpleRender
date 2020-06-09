#pragma once
#include "../math/Vector3f.h"
#include "../math/Mat4f_Transform.h"
#include "../shaders/BaseShader.h"
#include "FrameBuffer.h"
#include <QVector>

enum PolygonMode {Wire, Fill};
enum ShaderMode {Simple, Phong};
enum VertexMode {Triangle};


class Renderer {
public:
	// Renderer's setting
	PolygonMode m_polygon_mode;
	ShaderMode m_shader_mode;

	Mat4f m_viewport_matrix;
	Vector3f m_eye;
	// shader program includes light & material & texture & modle-world-view matrices
	BaseShader * m_shader_program = nullptr; 

	int width, height;
	bool do_depth_testing = true;
	bool do_back_face_culling = true;
	bool do_geometry_clipping = true;

	// all textures, all vertices, all 
	QVector<Texture2D *> m_texture_units;
	VertexMode m_vertex_mode;	// only support Triangle
	QVector<VertexIn> *m_vertices = nullptr;

	// frame buffer
	FrameBuffer * m_front_buffer = nullptr;
	FrameBuffer * m_back_buffer = nullptr;
	int num_triangles;
	int num_vertices;
public:
	Renderer(int width, int height);
	~Renderer();

	// status related
	void set_depth_testing(bool flag) { do_depth_testing = flag; }
	void set_back_face_culling(bool flag) { do_back_face_culling = flag; }
	void set_geometry_clipping(bool flag) { do_geometry_clipping = flag; }
	void set_polygon_mode(PolygonMode a) { m_polygon_mode = a; }
	void set_shader_mode(ShaderMode a); 
	void set_vertex_mode(VertexMode a) { m_vertex_mode = a; }

	// transfer vertices
	void set_vertices(QVector<VertexIn> *t_vertices) { m_vertices = t_vertices; }

	// matrix related
	void renderer_look_at(Vector3f &eye, Vector3f &up, Vector3f &center);
	void renderer_perspective(float fovy, float aspect, float near, float far);
	void renderer_ortho(float left, float right, float bottom, float top, float near, float far);
	void renderer_set_model_to_world_matrix(const Mat4f &model_to_world, const Mat4f &model_to_world_inverse_transposed);
	void renderer_viewport(int w, int h);

	// texture
	int load_texture_unit(Texture2D * unit);
	void bind_texture_unit(int texture_index);
	void unbind_texture_unit();

	// light & material
	void add_light(Light * light) { m_shader_program->add_light(light); }
	void clear_lights() { m_shader_program->clear_lights(); }
	void set_material(Material * material) { m_shader_program->set_material(material); }
public:
	// initialize & 
	void initialize(int w, int h);
	void swap_frame_buffer();
	void clear_back_frame_buffer(const Vector4f &background_color = Vector4f(0,0,0,1));
	void begin_frame();
	void end_frame();
	unsigned char * get_frame_image();

	// main
	void draw_mesh();
private:
	void projected_division(VertexOut &target);
	VertexOut lerp(const VertexOut &n1, const VertexOut &n2, double weight, bool is_final_result=false);
	bool line_cliping(const VertexOut &from, const VertexOut &to);
	bool triangle_cliping(const VertexOut &v1, const VertexOut &v2, const VertexOut &v3);
	bool z_culling(const Vector4f &v1, const Vector4f &v2, const Vector4f &v3);
	bool back_face_culling(const Vector4f &v1, const Vector4f &v2, const Vector4f &v3);
	void bresenham_line_rasterization(const VertexOut &from, const VertexOut &to);
	void scan_line_fill_per_row(const VertexOut &left, const VertexOut &right);
	void triangle_fill_rasterization(const VertexOut &v1, const VertexOut &v2, const VertexOut &v3);
};