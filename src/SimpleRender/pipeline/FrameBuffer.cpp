#include "FrameBuffer.h"
#include <qmath.h>

void FrameBuffer::init(int w, int h) {
	this->width = w;
	this->height = h;
	this->color_buffer = QVector<unsigned char>(4 * w * h);
	this->z_buffer = QVector<double>(w * h);

	clear_buffer();
}

void FrameBuffer::clear_buffer(Vector4f background_color) {
	unsigned char r = (qBound(0.f, 255.0f * background_color.data[0], 255.0f));
	unsigned char g = (qBound(0.f, 255.0f * background_color.data[1], 255.0f));
	unsigned char b = (qBound(0.f, 255.0f * background_color.data[2], 255.0f));
	unsigned char a = (qBound(0.f, 255.0f * background_color.data[3], 255.0f));

	int tmp;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			tmp = i * width + j;
			this->z_buffer[tmp] = MAX_Z;
			this->color_buffer[tmp * 4 + 0] = r;
			this->color_buffer[tmp * 4 + 1] = g;
			this->color_buffer[tmp * 4 + 2] = b;
			this->color_buffer[tmp * 4 + 3] = a;
		}
	}
}

void FrameBuffer::draw_pixel_color(int x, int y, Vector4f color) {
	// gamma correction
	unsigned char r = (qBound(0.f, 255.0f * color.data[0],  255.0f));
	unsigned char g = (qBound(0.f, 255.0f * color.data[1],  255.0f));
	unsigned char b = (qBound(0.f, 255.0f * color.data[2],  255.0f));
	unsigned char a = (qBound(0.f, 255.0f * color.data[3],  255.0f));

	int tmp = 4 * (y * width + x);
	color_buffer[tmp + 0] = r;
	color_buffer[tmp + 1] = g;
	color_buffer[tmp + 2] = b;
	color_buffer[tmp + 3] = a;
}

void FrameBuffer::draw_pixel_depth(int x, int y, double depth) {
	int tmp = y * width + x;
	z_buffer[tmp] = depth;
}

double FrameBuffer::get_depth(int x, int y) {
	int tmp = y * width + x;
	return z_buffer[tmp];
}

unsigned char * FrameBuffer::get_color_buffer() {
	return this->color_buffer.data();
}