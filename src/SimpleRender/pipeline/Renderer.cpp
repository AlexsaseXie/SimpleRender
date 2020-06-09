#include"Renderer.h"
#include"../math/Utils.h"
#include"../math/Vector4f.h"
#include"../math/Vector3f.h"
#include"../math/Vector2f.h"
#include"../shaders/ShaderVertexInOut.h"
#include"../shaders/SimpleShader.h"
#include"../shaders/PhongShader.h"

Renderer::Renderer(int w, int h) {
	this->initialize(w, h);
}

Renderer::~Renderer() {
	if (m_shader_program) delete m_shader_program;
	if (m_back_buffer) delete m_back_buffer;
	if (m_front_buffer) delete m_front_buffer;
}

// matrix related
void Renderer::renderer_look_at(Vector3f &eye, Vector3f &up, Vector3f &center) {
	m_eye = eye;
	Mat4f world_to_view;
	world_to_view.load_look_at(eye, up, center);
	
	m_shader_program->set_world_to_view_matrix(world_to_view);
	m_shader_program->set_eye(eye);
}

void Renderer::renderer_perspective(float fovy, float aspect, float near, float far) {
	Mat4f projection;
	projection.load_perspective(fovy, aspect, near, far);
	m_shader_program->set_projection_matrix(projection);
}

void Renderer::renderer_ortho(float left, float right, float bottom, float top, float near, float far) {
	Mat4f projection;
	projection.load_ortho(left, right, bottom, top, near, far);
	m_shader_program->set_projection_matrix(projection);
}

void Renderer::renderer_set_model_to_world_matrix(const Mat4f &model_to_world, const Mat4f &model_to_world_inverse_transposed) {
	m_shader_program->set_model_to_world_matrix(model_to_world);
	m_shader_program->set_model_to_world_inverse_transposed_matrix(model_to_world_inverse_transposed);
}

void Renderer::renderer_viewport(int w, int h) {
	this->width = w;
	this->height = h;

	this->m_viewport_matrix.load_viewport(0, 0, this->width, this->height);
}

// initialize & 
void Renderer::initialize(int w, int h) {
	this->width = w;
	this->height = h;
	this->renderer_viewport(w, h);
	//frame buffer
	m_eye = Vector3f(0, 0, -5);
	this->m_front_buffer = new FrameBuffer(w, h);
	this->m_back_buffer = new FrameBuffer(w, h);
	this->m_shader_program = nullptr;
	this->set_shader_mode(ShaderMode::Simple);
}

void Renderer::set_shader_mode(ShaderMode a) {
	m_shader_mode = a;
	if (m_shader_program!=nullptr) delete m_shader_program;
	if (m_shader_mode == ShaderMode::Simple)
		m_shader_program = new SimpleShader;
	else
		m_shader_program = new PhongShader;
}

void Renderer::swap_frame_buffer() {
	FrameBuffer * tmp = this->m_front_buffer;
	this->m_front_buffer = this->m_back_buffer;
	this->m_back_buffer = tmp;
}

void Renderer::clear_back_frame_buffer(const Vector4f &background_color) {
	m_back_buffer->clear_buffer(background_color);
}

void Renderer::begin_frame() {
	//pass
	num_triangles = 0;
	num_vertices = 0;
}

void Renderer::end_frame() {
	swap_frame_buffer();
	clear_back_frame_buffer();
}

unsigned char * Renderer::get_frame_image() {
	return m_front_buffer->get_color_buffer();
}

// main procedure
void Renderer::draw_mesh() {
	// m_vertex_mode == VertexMode::Triangle
	if (m_vertices->size() < 3) return;

	// For line cliping.
	bool line1 = false, line2 = false, line3 = false;
	
	int triangle_size = m_vertices->size() / 3;
	for (unsigned int i = 0; i < triangle_size; i += 1) {
		// assemble to a triangle primitive.
		VertexIn p1, p2, p3;
		p1 = (*m_vertices)[i * 3 + 0];
		p2 = (*m_vertices)[i * 3 + 1];
		p3 = (*m_vertices)[i * 3 + 2];

		// vertex shader stage.
		VertexOut v1, v2, v3;
		v1 = m_shader_program->vertex_shader(p1);
		v2 = m_shader_program->vertex_shader(p2);
		v3 = m_shader_program->vertex_shader(p3);
		
		// z culling
		if (!this->z_culling(v1.pos_projected, v2.pos_projected, v3.pos_projected))
			continue;

		// back face culling.
		if (do_back_face_culling && !this->back_face_culling(v1.pos_world, v2.pos_world, v3.pos_world))
			continue;

		// transfer to projection space
		projected_division(v1);
		projected_division(v2);
		projected_division(v3);

		// geometry cliping.
		if (do_geometry_clipping) {
			if (m_polygon_mode == PolygonMode::Wire) {
				line1 = line_cliping(v1, v2);
				line2 = line_cliping(v2, v3);
				line3 = line_cliping(v3, v1);
			}
			if (m_polygon_mode == PolygonMode::Fill && !triangle_cliping(v1, v2, v3))
				continue;
		}

		// viewport transform
		v1.pos_projected = m_viewport_matrix * v1.pos_projected;
		v2.pos_projected = m_viewport_matrix * v2.pos_projected;
		v3.pos_projected = m_viewport_matrix * v3.pos_projected;

		// rasterization and fragment shader stage.
		if (m_polygon_mode == PolygonMode::Wire) {
			if (!line1)
				bresenham_line_rasterization(v1, v2);
			if (!line2)
				bresenham_line_rasterization(v2, v3);
			if (!line3)
				bresenham_line_rasterization(v3, v1);
		}
		else if (m_polygon_mode == PolygonMode::Fill) {
			triangle_fill_rasterization(v1, v2, v3);
		}

		if (!line1 && !line2 && !line3)
			num_triangles ++;
		if (!line1)
			num_vertices ++;
		if (!line2)
			num_vertices ++;
		if (!line3)
			num_vertices ++;
	}
}

void Renderer::projected_division(VertexOut &target) {
	// record z in world space
	target.one_div_z =  1.0 / target.pos_projected.w();
	// perspective divison
	target.pos_projected.divide_by_w_inplace();
	// map from [-1,1] to [0,1]
	target.pos_projected.z() = ((double)target.pos_projected.z() * 0.5 + 0.5);

	// make sure pos_world & color & texture is correctly linear interpolated
	target.pos_world = target.pos_world * target.one_div_z;
	target.color = target.color * target.one_div_z;
	target.normal_world = target.normal_world * target.one_div_z;
	target.texture_coord = target.texture_coord * target.one_div_z;
}

VertexOut Renderer::lerp(const VertexOut &n1, const VertexOut &n2, double weight, bool is_final_result) {
	// linear interpolation, weight is in projection space
	VertexOut result;
	// pos_projected is linear interpolated in projection space
	result.pos_projected = linear_p(n1.pos_projected, n2.pos_projected, weight);

	// these coordinate need to be interpolated in view space
	result.pos_world = linear_p(n1.pos_world, n2.pos_world, weight);
	result.color = linear_p(n1.color, n2.color, weight);
	result.normal_world = linear_p(n1.normal_world, n2.normal_world, weight);
	result.texture_coord = linear_p(n1.texture_coord, n2.texture_coord, weight);
	result.one_div_z = n1.one_div_z * (1.0 - weight) + n2.one_div_z * weight;

	// return correct linear interpolation in view space
	if (is_final_result) {
		double tmp = 1. / result.one_div_z;
		result.pos_world = result.pos_world * tmp;
		result.color = result.color * tmp;
		result.normal_world = result.normal_world * tmp;
		result.texture_coord = result.texture_coord * tmp;
	}
	return result;
}

bool Renderer::line_cliping(const VertexOut &from, const VertexOut &to) {
	// Cohen-Sutherland algorithm.
	// return whether the line is totally outside or not.

	float vMin = -1.0, vMax = +1.0;
	float x1 = from.pos_projected.data[0], y1 = from.pos_projected.data[1];
	float x2 = to.pos_projected.data[0], y2 = to.pos_projected.data[1];
	float z1 = from.pos_projected.data[2], z2 = to.pos_projected.data[2];

	const float extreme = 10;
	if (qAbs(z1) >= extreme || qAbs(z2) >= extreme)
		return true;

	int tmp = 0;
	int outcode1 = 0, outcode2 = 0;

	// outcode1 calculation.
	tmp = (y1>vMax) ? 1 : 0;
	tmp <<= 3;
	outcode1 |= tmp;
	tmp = (y1<vMin) ? 1 : 0;
	tmp <<= 2;
	outcode1 |= tmp;
	tmp = (x1>vMax) ? 1 : 0;
	tmp <<= 1;
	outcode1 |= tmp;
	tmp = (x1<vMin) ? 1 : 0;
	outcode1 |= tmp;

	// outcode2 calculation.
	tmp = (y2>vMax) ? 1 : 0;
	tmp <<= 3;
	outcode2 |= tmp;
	tmp = (y2<vMin) ? 1 : 0;
	tmp <<= 2;
	outcode2 |= tmp;
	tmp = (x2>vMax) ? 1 : 0;
	tmp <<= 1;
	outcode2 |= tmp;
	tmp = (x2<vMin) ? 1 : 0;
	outcode2 |= tmp;

	if ((outcode1 & outcode2) != 0)
		return true;

	// bounding box judge.
	Vector2f minPoint, maxPoint;
	minPoint.x() = qMin(x1, x2);
	minPoint.y() = qMin(y1, y2);
	maxPoint.x() = qMax(x1, x2);
	maxPoint.y() = qMax(y1, y2);
	if (minPoint.x() > vMax || maxPoint.x() < vMin || minPoint.y() > vMax || maxPoint.y() < vMin)
		return true;

	return false;
}

bool Renderer::triangle_cliping(const VertexOut &v1, const VertexOut &v2, const VertexOut &v3) {
	// perform a easier clip
	float vMin = -1.0;
	float vMax = +1.0;

	// if the triangle is too far to see it, just return false.
	if (v1.pos_projected.data[2] > vMax && v2.pos_projected.data[2] > vMax && v3.pos_projected.data[2] > vMax)
		return false;

	// if the triangle is behind the camera, just return false.
	if (v1.pos_projected.data[2] < vMin && v2.pos_projected.data[2] < vMin && v3.pos_projected.data[2] < vMin)
		return false;

	// calculate the bounding box and check if clip or not.
	Vector2f minPoint, maxPoint;
	minPoint.x() = qMin(v1.pos_projected.data[0], qMin(v2.pos_projected.data[0], v3.pos_projected.data[0]));
	minPoint.y() = qMin(v1.pos_projected.data[1], qMin(v2.pos_projected.data[1], v3.pos_projected.data[1]));
	maxPoint.x() = qMax(v1.pos_projected.data[0], qMax(v2.pos_projected.data[0], v3.pos_projected.data[0]));
	maxPoint.y() = qMax(v1.pos_projected.data[1], qMax(v2.pos_projected.data[1], v3.pos_projected.data[1]));
	if (minPoint.x() > vMax || maxPoint.x() < vMin || minPoint.y() > vMax || maxPoint.y() < vMin)
		return false;

	return true;
}

bool Renderer::z_culling(const Vector4f &v1, const Vector4f &v2, const Vector4f &v3) {
	float z = v1.data[2];
	float w = v1.data[3];
	if (qAbs(z) > qAbs(w))
		return false;

	z = v2.data[2];
	w = v2.data[3];
	if (qAbs(z) > qAbs(w))
		return false;

	z = v3.data[2];
	w = v3.data[3];
	if (qAbs(z) > qAbs(w))
		return false;

	return true;
}

bool Renderer::back_face_culling(const Vector4f &v1, const Vector4f &v2, const Vector4f &v3) {
	// back faces culling.
	Vector4f tmp1 = v2 - v1;
	Vector4f tmp2 = v3 - v1;
	Vector3f edge1(tmp1.x(), tmp1.y(), tmp1.z());
	Vector3f edge2(tmp2.x(), tmp2.y(), tmp2.z());
	Vector3f view_ray(m_eye.x() - v1.data[0], m_eye.y() - v1.data[1], m_eye.z() - v1.data[2]);
	Vector3f normal = cross(edge1, edge2);
	return dot(normal,view_ray) >= 0;
}

void Renderer::bresenham_line_rasterization(const VertexOut &from, const VertexOut &to) {
	int x1 = qRound(from.pos_projected.data[0]);
	int y1 = qRound(from.pos_projected.data[1]);
	int x2 = qRound(to.pos_projected.data[0]);
	int y2 = qRound(to.pos_projected.data[1]);

	// check invalid
	if ((x1 < 0 && x2 < 0) || (x1 >= width && x2 >= width))
		return;
	if ((y1 < 0 && y2 < 0) || (y1 >= height && y2 >= height))
		return;

	// too small to draw
	if (x1 == x2 && y1 == y2)
		return;

	int dx = x2 - x1;
	int dy = y2 - y1;
	int ux = 1, uy = 1;

	// judge the sign
	if (dx < 0) {
		ux = -1;
		dx = -dx;
	}
	if (dy < 0) {
		uy = -1;
		dy = -dy;
	}

	int x = x1, y = y1;
	int error = 0;

	VertexOut tmp;
	double weight;
	double weight_ratio;
	if (dy < dx) {
		// slope < 1.
		int x1_it, x2_it;
		if (ux > 0) {
			x1_it = x1 > 0 ? x1 : 0;
			x2_it = x2 < width - 1 ? x2 : width - 1;
		}
		else {
			x1_it = x1 < width - 1 ? x1 : width - 1;
			x2_it = x2 > 0 ? x2 : 0;
		}
		weight = (double)(ux > 0 ? x1_it - x1 : x1 - x1_it) / (double)dx;		
		weight_ratio = 1. / (double)dx;
		// compute initial y & error 
		tmp = lerp(from, to, weight, true);
		y = qRound(tmp.pos_projected.y());
		error = (long long)qAbs(x1_it - x1) * dy - (long long)qAbs(y - y1) * dx;
		//weight = 0;
		//weight_ratio = 1. / (double)dx;
		for (x = x1_it; x != x2_it + ux; x += ux, weight += weight_ratio) {
			// linear interpolation
			if (x >= 0 && x <= width - 1 && y >= 0 && y <= height - 1) {			
				tmp = lerp(from, to, weight, true);
				if (tmp.pos_projected.z() >= 0 && tmp.pos_projected.z() <= 1) {
					if (do_depth_testing) {
						// depth testing.
						double depth = m_back_buffer->get_depth(x, y);
						if (tmp.pos_projected.z() <= depth) {
							m_back_buffer->draw_pixel_depth(x, y, tmp.pos_projected.z());
							// fragment shader
							m_back_buffer->draw_pixel_color(x, y, m_shader_program->fragment_shader(tmp));
						}
					}
					else
						m_back_buffer->draw_pixel_color(x, y, Vector4f(0, 0, 1, 1));
				}
			}

			// error update
			error += dy;
			if ((error << 1) >= dx) {
				y += uy;
				error -= dx;
			}
		}
	}
	else {
		// slope >= 1.
		int y1_it, y2_it;
		if (ux > 0) {
			y1_it = y1 > 0 ? y1 : 0;
			y2_it = y2 < height - 1 ? y2 : height - 1;
		}
		else {
			y1_it = y1 < height - 1 ? y1 : height - 1;
			y2_it = y2 > 0 ? y2 : 0;
		}

		weight = (double)(uy > 0 ? y1_it - y1 : y1 - y1_it) / (double)dy;
		weight_ratio = 1. / (double)dy;

		// compute initial y & error 
		tmp = lerp(from, to, weight, true);
		x = qRound(tmp.pos_projected.x());
		error = (long long)qAbs(y1_it - y1) * dx - (long long)qAbs(x - x1) * dy;
		//weight = 0;
		//weight_ratio = 1. / (double)dy;
		for (y = y1_it; y != y2_it + uy; y += uy, weight += weight_ratio) {
			// linear interpolation
			if (x >= 0 && x <= width - 1 && y >= 0 && y <= height - 1) {			
				tmp = lerp(from, to, weight, true);
				if (tmp.pos_projected.z() >= 0 && tmp.pos_projected.z() <= 1) {
					if (do_depth_testing) {
						// depth testing.
						double depth = m_back_buffer->get_depth(x, y);
						if (tmp.pos_projected.z() <= depth) {
							m_back_buffer->draw_pixel_depth(x, y, tmp.pos_projected.z());
							// fragment shader
							m_back_buffer->draw_pixel_color(x, y, m_shader_program->fragment_shader(tmp));
						}
					}
					else
						m_back_buffer->draw_pixel_color(x, y, Vector4f(0, 0, 1, 1));
				}
			}

			// error update
			error += dx;
			if ((error << 1) >= dy) {
				x += ux;
				error -= dy;
			}
		}
	}
}

void Renderer::scan_line_fill_per_row(const VertexOut &left, const VertexOut &right) {
	// scan the line from left to right.
	VertexOut current;
	int y = left.pos_projected.data[1]; // should be an integer already
	if (y < 0 || y >= height)
		return;

	int x1 = qRound(left.pos_projected.data[0]);
	int x2 = qRound(right.pos_projected.data[0]);
	double dx = right.pos_projected.data[0] - left.pos_projected.data[0];

	int x1_it = x1 > 0 ? x1 : 0;
	int x2_it = x2 < width - 1 ? x2 : width - 1;
	int x;
	double weight = (double)(x1_it - x1) / dx;
	double weight_ratio = 1.0 / dx;
	for (x = x1_it; x <= x2_it; x++, weight += weight_ratio) {
		// linear interpolation
		current = lerp(left, right, weight, true);

		// don't need to check if (x, y) is in the viewport, already ensure that
		if (do_depth_testing) {
			// depth testing.
			double depth = m_back_buffer->get_depth(x, y);
			if (current.pos_projected.z() <= depth) {
				m_back_buffer->draw_pixel_depth(x, y, current.pos_projected.z());
				// fragment shader
				m_back_buffer->draw_pixel_color(x, y, m_shader_program->fragment_shader(current));
			}
		}
		else
			m_back_buffer->draw_pixel_color(x, y, m_shader_program->fragment_shader(current));
	}
}

void Renderer::triangle_fill_rasterization(const VertexOut &v1, const VertexOut &v2, const VertexOut &v3) {
	// split the triangle into two part
	VertexOut tmp;
	VertexOut target[3] = { v1, v2, v3 };
	// sort points by y
	if (target[0].pos_projected.y() > target[1].pos_projected.y() ) {
		tmp = target[0];
		target[0] = target[1];
		target[1] = tmp;
	}
	if (target[0].pos_projected.y() > target[2].pos_projected.y()) {
		tmp = target[0];
		target[0] = target[2];
		target[2] = tmp;
	}
	if (target[1].pos_projected.y() > target[2].pos_projected.y()) {
		tmp = target[1];
		target[1] = target[2];
		target[2] = tmp;
	}

	Vector3f v1_v0 = Vector3f(target[1].pos_projected.x(), target[1].pos_projected.y(), 0) - Vector3f(target[0].pos_projected.x(), target[0].pos_projected.y(), 0);
	Vector3f v2_v0 = Vector3f(target[2].pos_projected.x(), target[2].pos_projected.y(), 0) - Vector3f(target[0].pos_projected.x(), target[0].pos_projected.y(), 0);
	Vector3f cross_z = cross(v1_v0, v2_v0);
	if (equal(cross_z.z(), 0.0f))
		return;

	int y_start = qRound(target[0].pos_projected.y());
	int y_end = qRound(target[2].pos_projected.y());
	int y_mid = qRound(target[1].pos_projected.y());

	double weight_v1 = 0;
	double weight_v2 = 0;
	double weight_ratio_v1;
	double weight_ratio_v2;
	if (y_start >= height)
		return;
	int y_iter_start = y_start > 0 ? y_start : 0;
	int y_iter_end = y_end < height - 1 ? y_end : height - 1;

	if (y_iter_start < y_mid) {
		weight_ratio_v1 = 1.0 / (double)(y_mid - y_start);
		weight_v1 = (double)(y_iter_start - y_start) * weight_ratio_v1;
	}
	else {
		weight_ratio_v1 = 1.0 / (double)(y_end - y_mid);
		weight_v1 = (double)(y_iter_start - y_mid) * weight_ratio_v1;
	}

	weight_ratio_v2 = 1.0 / (double)(y_end - y_start);
	weight_v2 = (double)(y_iter_start - y_start) * weight_ratio_v2;
	int y;
	VertexOut scan_line_v1, scan_line_v2;
	bool scan_line_v1_on_the_left = (cross_z.z() < 0);
	for (y = y_iter_start; y <= y_iter_end; y++) {
		if (y == y_mid) {
			// change v1
			weight_v1 = 0;
			weight_ratio_v1 = 1.0 / (double)(y_end - y_mid);
		}

		if (y < y_mid)
			scan_line_v1 = lerp(target[0], target[1], weight_v1, false);
		else 
			scan_line_v1 = lerp(target[1], target[2], weight_v1, false);
		scan_line_v2 = lerp(target[0], target[2], weight_v2, false);

		scan_line_v1.pos_projected.y() = y;
		scan_line_v2.pos_projected.y() = y;

		if (!scan_line_v1_on_the_left) {
			tmp = scan_line_v1;
			scan_line_v1 = scan_line_v2;
			scan_line_v2 = tmp;
		}

		// v1.x should be smaller than v2.x
		if (scan_line_v1.pos_projected.x() <= scan_line_v2.pos_projected.x()) {
			scan_line_fill_per_row(scan_line_v1, scan_line_v2);
		}
		
		weight_v1 += weight_ratio_v1;
		weight_v2 += weight_ratio_v2;
	}
}

// texture
int Renderer::load_texture_unit(Texture2D * unit) {
	int count = m_texture_units.size();
	m_texture_units.push_back(unit);
	return count;
}

void Renderer::bind_texture_unit(int texture_index) {
	m_shader_program->bind_texture_unit(m_texture_units[texture_index]);
}

void Renderer::unbind_texture_unit() {
	m_shader_program->unbind_texture_unit();
}