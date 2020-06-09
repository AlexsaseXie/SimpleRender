#pragma once
#include <QImage>
#include <QString>
#include "../math/Vector2f.h"
#include "../math/Vector4f.h"


class Texture2D	{
public:
	QImage image;
	int width;
	int height;
public:
	Texture2D() {}
	Texture2D(const QString file) { load_from_file(file); }
	void load_from_file(const QString file);
	Vector4f sample(const Vector2f &texcoord);
};
