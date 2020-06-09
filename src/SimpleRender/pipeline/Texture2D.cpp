#include "Texture2D.h"

#include <qmath.h>

void Texture2D::load_from_file(const QString file) {
	image = QImage(file).convertToFormat(QImage::Format::Format_RGBA8888);

	width = image.width();
	height = image.height();
}


Vector4f Texture2D::sample(const Vector2f &texcoord) {
	float x = texcoord.data[0];
	float y = texcoord.data[1];

	if (x > 1.0f)
		x = x - qFloor(x);
	if (y > 1.0f)
		y = y - qFloor(y);

	float image_x = x * (float)(width - 1);
	float image_y = y * (float)(height - 1);

	int image_x_coor = qRound(image_x);
	int image_y_coor = qRound(image_y);
	QColor tmp = image.pixel(image_x_coor, image_y_coor);
	Vector4f tmp_color = Vector4f((float)tmp.red(), (float)tmp.green(), (float)tmp.blue(), (float)tmp.alpha());
	return tmp_color * (1.0 / 255.0);

	/*
	int image_x_floor = qFloor(image_x);
	int image_y_floor = qFloor(image_y);
	int image_x_ceil = qCeil(image_x);
	int image_y_ceil = qCeil(image_x);

	double factor_left = image_x - (float)image_x_floor;
	double factor_top = image_y - (float)image_y_floor;
	// left top
	QColor left_top = image.pixel(image_x_floor, image_y_floor);
	QColor left_bottom = image.pixel(image_x_floor, image_y_ceil);
	QColor right_top = image.pixel(image_x_ceil, image_y_floor);
	QColor right_bottom = image.pixel(image_x_ceil, image_y_ceil);

	Vector4f left_top_color = Vector4f((float)left_top.red(), (float)left_top.green(), (float)left_top.blue(), (float)left_top.alpha());
	return left_top_color * (1.0 / 255.0);
	
	Vector4f left_bottom_color = Vector4f((float)left_bottom.red(), (float)left_bottom.green(), (float)left_bottom.blue(), (float)left_bottom.alpha());
	Vector4f right_top_color = Vector4f((float)right_top.red(), (float)right_top.green(), (float)right_top.blue(), (float)right_top.alpha());
	Vector4f right_bottom_color = Vector4f((float)right_bottom.red(), (float)right_bottom.green(), (float)right_bottom.blue(), (float)right_bottom.alpha());

	Vector4f left_color = linear_p(left_top_color, left_bottom_color, factor_top);
	Vector4f right_color = linear_p(right_top_color, right_bottom_color, factor_top);
	Vector4f result = linear_p(left_color, right_color, factor_left) * (1.0 / 255.0);

	return result;
	*/
}