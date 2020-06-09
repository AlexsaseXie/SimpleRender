#pragma once

#include <QObject>
#include "pipeline/Scene.h"
#include "pipeline/Renderer.h"
#include "pipeline/Texture2D.h"

class RenderLoop : public QObject
{
	Q_OBJECT
public:
	explicit RenderLoop(int w, int h, QObject *parent = nullptr);
	~RenderLoop();

	inline void stop() { stopped = true; }
	inline void set_fps_to_zero() { fps = 0; time_duration = 0; }
	inline int get_fps() { return fps; }

	void load_simple_scene();
	void load_scene();
	void prepare_vertices_in();

public:
	// recv event
	void press_key(char key);
	void mouse_move_horizontal(int delta);
	void mouse_move_vertical(int delta);
	void roll_wheel(int delta);
	void view_rotate(int z);

signals:
	void frameOut(unsigned char *image, int num_triangles, int num_vertices);

public slots:
	void render_loop();	
	void change_render(int a, bool flag);
	void change_projection(int a, bool flag);
	void change_texture(QString path);

private:
	bool stopped;
	Texture2D * current_texture;
	int current_texture_int;
	int fps;
	double frame_time;
	Scene scene;
	Renderer * renderer;
	Material * material;
	QVector<QVector<VertexIn>> vertices_in;
	int width;
	int height;

	int time_duration;
	int time_start;
};
