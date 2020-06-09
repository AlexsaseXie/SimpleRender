#include <time.h>
#include "RenderLoop.h"
#include "pipeline/Light_Material.h"

RenderLoop::RenderLoop(int w, int h, QObject *parent) :
	QObject(parent) {
	width = w;
	height = h;
	fps = 0;
	frame_time = 0;
	stopped = false;
	renderer = new Renderer(w, h);
}

RenderLoop::~RenderLoop() {
	delete renderer;
}

void RenderLoop::load_simple_scene() {
	scene.camera.eye = Vector3f(0, 0, -5);
	scene.camera.center = Vector3f(0, 0, 0);

	int n = 4;
	this->scene.all_mesh = QVector<Mesh>(n);
	this->scene.all_mesh_transform = QVector<Transform3D>(n);
	
	// material & lights
	material = new Material(
		Vector3f(0.5, 0.5, 0.5),
		Vector3f(0.3, 0.3, 0.3),
		Vector3f(0.6, 0.6, 0.6),
		40.0
	);
	int n_lights = 1;
	this->scene.all_lights = QVector<Light *>(n_lights);
	this->scene.all_lights[0] = new DirectionalLight(
		Vector3f(-0.3, 0.3, 1),
		Vector3f(0.8, 0.8, 0.8),
		Vector3f(0.3, 0.3, 0.3)
	);

	/*this->scene.all_lights[1] = new PointLight(
		Vector3f(-3.0, 0.0, 0.0),
		Vector3f(0.8, 0.8, 0.8),
		Vector3f(0.3, 0.3, 0.3),
		Vector3f(1.0f, 0.007f, 0.0017f)
	);*/

	// ground
	{
		this->scene.all_mesh[0].insert_point(-3, -3, 5);
		this->scene.all_mesh[0].insert_point(3, -3, 5);
		this->scene.all_mesh[0].insert_point(3, 3, 5);
		this->scene.all_mesh[0].insert_point(-3, 3, 5);
		this->scene.all_mesh[0].insert_texture(0., 0.);
		this->scene.all_mesh[0].insert_texture(1., 0.);
		this->scene.all_mesh[0].insert_texture(1., 1.);
		this->scene.all_mesh[0].insert_texture(0., 1.);
		QVector<FaceIndex> tmp;
		tmp.append(FaceIndex(0, 0, -1));
		tmp.append(FaceIndex(3, 3, -1));
		tmp.append(FaceIndex(2, 2, -1));
		this->scene.all_mesh[0].insert_face(tmp);
		tmp.clear();
		tmp.append(FaceIndex(0, 0, -1));
		tmp.append(FaceIndex(2, 2, -1));
		tmp.append(FaceIndex(1, 1, -1));
		this->scene.all_mesh[0].insert_face(tmp);
		this->scene.all_mesh[0].calc_normals(false);
	}

	for (int i = 1; i < 4; i++) {
		if (i == 1)
			this->scene.all_mesh[i].load_mesh_from_file("data/monkey.obj");
		else 
			this->scene.all_mesh[i].load_mesh_from_file("data/cube.obj");
		
		this->scene.all_mesh[i].normalize();

		if (i == 1)
			this->scene.all_mesh[i].calc_normals(true);
		else
			this->scene.all_mesh[i].calc_normals(false);
	}

	this->scene.all_mesh_transform[2].set_translation(Vector3f(2, 0, 0));
	this->scene.all_mesh_transform[2].set_scale(Vector3f(1.5, 1.5, 1.5));
	this->scene.all_mesh_transform[3].set_translation(Vector3f(0, 1.5, 0));
	this->scene.all_mesh_transform[3].set_rotate(Vector3f(1, 1, 1), 30.);
}

void RenderLoop::load_scene() {
	scene.camera.eye = Vector3f(0, 0, -5);
	scene.camera.center = Vector3f(0, 0, 0);

	int n = 4;
	this->scene.all_mesh = QVector<Mesh>(n);
	this->scene.all_mesh_transform = QVector<Transform3D>(n);

	// material & lights
	material = new Material(
		Vector3f(0.5, 0.5, 0.5),
		Vector3f(0.6, 0.6, 0.6),
		Vector3f(0.6, 0.6, 0.6),
		40.0
	);
	int n_lights = 1;
	this->scene.all_lights = QVector<Light *>(n_lights);
	/*this->scene.all_lights[0] = new DirectionalLight(
		Vector3f(-0.3, 0.3, 1),
		Vector3f(0.8, 0.8, 0.8),
		Vector3f(0.3, 0.3, 0.3)
	);*/

	this->scene.all_lights[0] = new PointLight(
		Vector3f(-3.0, 0.0, -3.0),
		Vector3f(1.0, 1.0, 1.0),
		Vector3f(0.3, 0.3, 0.3),
		Vector3f(1.0f, 0.001f, 0.001f)
	);

	// ground
	{
		this->scene.all_mesh[0].insert_point(-3, -3, 5);
		this->scene.all_mesh[0].insert_point(3, -3, 5);
		this->scene.all_mesh[0].insert_point(3, 3, 5);
		this->scene.all_mesh[0].insert_point(-3, 3, 5);
		this->scene.all_mesh[0].insert_texture(0., 0.);
		this->scene.all_mesh[0].insert_texture(1., 0.);
		this->scene.all_mesh[0].insert_texture(1., 1.);
		this->scene.all_mesh[0].insert_texture(0., 1.);
		QVector<FaceIndex> tmp;
		tmp.append(FaceIndex(0, 0, -1));
		tmp.append(FaceIndex(3, 3, -1));
		tmp.append(FaceIndex(2, 2, -1));
		this->scene.all_mesh[0].insert_face(tmp);
		tmp.clear();
		tmp.append(FaceIndex(0, 0, -1));
		tmp.append(FaceIndex(2, 2, -1));
		tmp.append(FaceIndex(1, 1, -1));
		this->scene.all_mesh[0].insert_face(tmp);
		this->scene.all_mesh[0].calc_normals(false);
	}

	this->scene.all_mesh[1].load_mesh_from_file("data/monkey.obj");
	this->scene.all_mesh[1].normalize();
	this->scene.all_mesh[1].calc_normals(true);
	this->scene.all_mesh[1].sphere_texture_mapping();

	this->scene.all_mesh[2].load_mesh_from_file("data/cube.obj");
	this->scene.all_mesh[2].normalize();
	this->scene.all_mesh[2].calc_normals(false);	
	this->scene.all_mesh[2].sphere_texture_mapping();

	this->scene.all_mesh[3].load_mesh_from_file("data/cow.obj");
	this->scene.all_mesh[3].normalize();
	this->scene.all_mesh[3].calc_normals(true);

	this->scene.all_mesh_transform[2].set_translation(Vector3f(2, 0, 0));
	this->scene.all_mesh_transform[2].set_scale(Vector3f(1.5, 1.5, 1.5));
	this->scene.all_mesh_transform[3].set_translation(Vector3f(0, 1.5, 0));
	this->scene.all_mesh_transform[3].set_scale(Vector3f(1.5, 1.5, 1.5));
	this->scene.all_mesh_transform[3].set_rotate(Vector3f(1, 1, 1), 30.);
}

void RenderLoop::prepare_vertices_in() {
	int mesh_count = this->scene.all_mesh.size();
	this->vertices_in = QVector<QVector<VertexIn>>(mesh_count);
	
	for (int i = 0; i < mesh_count; i++) {
		Mesh &mesh = this->scene.all_mesh[i];
		for (auto &f : mesh.faces) {
			this->vertices_in[i].push_back(VertexIn(
				mesh.vertexs[f[0].vertex_index], 
				Vector4f(1, 0, 0, 1), 
				f[0].texture_index >= 0 ? mesh.textures[f[0].texture_index] : Vector2f(0.0f,0.0f), 
				mesh.normals[f[0].normal_index]
			));
			this->vertices_in[i].push_back(VertexIn(
				mesh.vertexs[f[1].vertex_index],
				Vector4f(0, 1, 0, 1),
				f[1].texture_index >= 0 ? mesh.textures[f[1].texture_index] : Vector2f(0.0f, 0.0f),
				mesh.normals[f[1].normal_index]
			)); 
			this->vertices_in[i].push_back(VertexIn(
				mesh.vertexs[f[2].vertex_index],
				Vector4f(0, 0, 1, 1),
				f[2].texture_index >= 0 ? mesh.textures[f[2].texture_index] : Vector2f(0.0f, 0.0f),
				mesh.normals[f[2].normal_index]
			));
		}
	}
}

void RenderLoop::render_loop() {
	//load_simple_scene();
	load_scene();
	prepare_vertices_in();

	// init shader
	this->renderer->set_shader_mode(ShaderMode::Phong);

	// material & lights
	this->renderer->set_material(material);
	for (auto &li : this->scene.all_lights) {
		this->renderer->add_light(li);
	}

	// set matrices
	this->renderer->renderer_perspective(45.0f, (double)width / (double)height, 0.1f, 40.f);
	this->renderer->renderer_viewport(width, height);
	this->renderer->set_back_face_culling(true);
	this->renderer->set_depth_testing(true);
	this->renderer->set_geometry_clipping(true);
	this->renderer->set_polygon_mode(PolygonMode::Fill);
	this->renderer->set_vertex_mode(VertexMode::Triangle);
	// texture
	current_texture = new Texture2D("data/texture1.bmp");
	current_texture_int = this->renderer->load_texture_unit(current_texture);

	int mesh_count = scene.all_mesh.size();
	fps = 0;
	time_duration = 0;
	const double ps_duration = (double)CLOCKS_PER_SEC / 80.;
	while (!stopped) {
		time_start = clock();
		this->renderer->renderer_look_at(scene.camera.eye, scene.camera.up, scene.camera.center);
		this->renderer->begin_frame();

		for (int i = 0; i < mesh_count; i++) {
			if (i == 0 || i == 1 || i == 2)
				this->renderer->bind_texture_unit(current_texture_int);

			this->renderer->set_vertices(&this->vertices_in[i]);
			this->renderer->renderer_set_model_to_world_matrix(
				this->scene.all_mesh_transform[i].to_matrix(), 
				this->scene.all_mesh_transform[i].to_inverse_transposed_matrix()
			);
			// raseterize
			this->renderer->draw_mesh();
				
			this->renderer->unbind_texture_unit();
		}

		this->renderer->end_frame();
		unsigned char * image = this->renderer->get_frame_image();

		time_duration += clock() - time_start;
		if (time_duration >= (double)(fps + 1) * ps_duration) {
			emit frameOut(image, this->renderer->num_triangles, this->renderer->num_vertices);
			fps++;
		}
	}
}

void RenderLoop::press_key(char key) {
	float speed = 0.2;
	switch (key)
	{
	case 'W':
		scene.camera.move_up(speed);
		break;
	case 'A':
		scene.camera.move_left(speed);
		break;
	case 'S':
		scene.camera.move_down(speed);
		break;
	case 'D':
		scene.camera.move_right(speed);
		break;
	default:
		break;
	}
}

void RenderLoop::mouse_move_vertical(int delta) {
	float speed = 0.3;
	scene.camera.rotate_up((float)delta * speed);
}

void RenderLoop::mouse_move_horizontal(int delta) {
	float speed = 0.3;
	scene.camera.rotate_left((float)delta * speed);
}

void RenderLoop::roll_wheel(int delta) {
	float speed = 0.01;
	scene.camera.move_near((float)delta * speed);
}

void RenderLoop::view_rotate(int z) {
	if (z > 0)
		scene.camera.view_rotate(-1.);
	else if (z < 0)
		scene.camera.view_rotate(1.);
}

void RenderLoop::change_render(int a, bool flag) {
	if (a == 0)
		renderer->set_polygon_mode(PolygonMode::Wire);
	else
		renderer->set_polygon_mode(PolygonMode::Fill);
}

void RenderLoop::change_projection(int a, bool flag) {
	if (a == 0)
		this->renderer->renderer_perspective(45.0f, (double)width / (double)height, 0.1f, 40.f);
	else
		this->renderer->renderer_ortho(-3.f, 3.f, -3.f, 3.f, 0.01f, 40.0f);
}

void RenderLoop::change_texture(QString path) {
	Texture2D * new_unit = new Texture2D(path);
	current_texture_int = this->renderer->load_texture_unit(new_unit);
}