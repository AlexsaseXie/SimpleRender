#pragma once

#include<QVector>
#include<QString>
#include<QMessageBox>
#include"../math/Vector3f.h"
#include"../math/Vector2f.h"
#include"../shaders/ShaderVertexInOut.h"

#define Point3 Vector3f
#define Texture Vector2f
#define Normal Vector3f

class FaceIndex {
public:
	int vertex_index = -1;
	int texture_index = -1;
	int normal_index = -1;
public:
	inline FaceIndex() {}
	inline FaceIndex(int vi) { vertex_index = vi; texture_index = -1; normal_index = -1; }
	inline FaceIndex(int vi, int ti, int ni) { vertex_index = vi; texture_index = ti; normal_index = ni; }
};

class Mesh {
public:
	QVector<Point3> vertexs;
	QVector<Texture> textures;
	QVector<Normal> normals;
	QVector<QVector<FaceIndex>> faces;

	float max[3];
	float min[3];

public:
	// load related
	QString path;
	inline void load_mesh_from_file(QString m_path) {
		path = m_path;
		if (this->path.endsWith(".ply"))
			load_mesh_from_ply();
		else if (this->path.endsWith(".obj"))
			load_mesh_from_obj();
		else
			QMessageBox::warning(NULL, "Loading Failed!", "Model file format not supported.");
	}
	void load_mesh_from_ply();
	void load_mesh_from_obj();

public:
	void normalize();
	void normalize(float new_min[3], float new_max[3]);
	void calc_normals(bool smooth=true);
	void sphere_texture_mapping();

public:
	inline void insert_point(double x, double y, double z) {
		Point3 tmp = Vector3f(x, y, z);
		this->vertexs.push_back(tmp);
	}
	inline void insert_texture(double vu, double vf) {
		Texture tmp = Vector2f(vu, vf);
		this->textures.push_back(tmp);
	}
	inline void insert_normal(double x, double y, double z) {
		Normal tmp = Vector3f(x, y, z);
		this->normals.push_back(tmp);
	}
	inline void insert_face(QVector<int> &f) {
		QVector<FaceIndex> tmp;
		for (auto &i : f) {
			tmp.push_back(FaceIndex(i));
		}

		faces.push_back(tmp);
	}
	inline void insert_face(QVector<FaceIndex> &f) {
		faces.push_back(f);
	}
};