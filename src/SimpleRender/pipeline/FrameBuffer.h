#pragma once
#include<QVector>
#include "../math/Vector4f.h"

#define MAX_Z 1e10

class FrameBuffer {
public:
	int width;
	int height;
	QVector<double> z_buffer;
	QVector<unsigned char> color_buffer;
public:
	FrameBuffer(int w, int h) { 
		init(w, h);
	}
	~FrameBuffer() {}
public:
	void init(int w, int h);
	void clear_buffer(Vector4f background_color = Vector4f(0.,0.,0.,1.0));
	void draw_pixel_color(int x, int y, Vector4f color);
	void draw_pixel_depth(int x, int y, double depth);
	double get_depth(int x, int y);

public:
	unsigned char * get_color_buffer();
};