#include "Mesh.h"
#include <QFile>
#include <qmath.h>

void Mesh::normalize() {
	// centered at (0,0,0), max length = 1
	float model_l = this->max[0] - this->min[0];
	model_l = this->max[1] - this->min[1] > model_l ? this->max[1] - this->min[1] : model_l;
	model_l = this->max[2] - this->max[2] > model_l ? this->max[2] - this->min[2] : model_l;

	float new_max[3] = { 
		0.5f * (this->max[0] - this->min[0]) / model_l, 
		0.5f * (this->max[1] - this->min[1]) / model_l, 
		0.5f * (this->max[2] - this->min[2]) / model_l,
	};

	float new_min[3];
	for (int i = 0; i < 3; i++)
		new_min[i] = -new_max[i];

	this->normalize(new_min, new_max);
}

void Mesh::normalize(float new_min[3], float new_max[3]) {
	float f[3];
	for (int i = 0; i < 3; i++) {
		f[i] = (new_max[i] - new_min[i]) / (max[i] - min[i]);
	}

	// apply transfer

	for (auto &v : vertexs) {
		float p[3] = { v.x(), v.y(), v.z() };

		for (int i = 0; i < 3; i++) {
			p[i] = (p[i] - min[i]) * f[i] + new_min[i];
		}

		v.x() = p[0];
		v.y() = p[1];
		v.z() = p[2];
	}

	for (int i = 0; i < 3; i++) {
		max[i] = new_max[i];
		min[i] = new_min[i];
	}
}

void Mesh::calc_normals(bool smooth) {
	if (this->faces[0][0].normal_index != -1)
		return;

	if (smooth) {
		int v_count = this->vertexs.size();
		this->normals = QVector<Normal>(v_count, Normal(0, 0, 0));
		for (auto &f : this->faces) {
			f[0].normal_index = f[0].vertex_index;
			f[1].normal_index = f[1].vertex_index;
			f[2].normal_index = f[2].vertex_index;

			Vector3f v0_v1 = this->vertexs[f[1].vertex_index] - this->vertexs[f[0].vertex_index];
			Vector3f v0_v2 = this->vertexs[f[2].vertex_index] - this->vertexs[f[0].vertex_index];

			Vector3f up = ::normalize(cross(v0_v1, v0_v2));

			this->normals[f[0].normal_index] = this->normals[f[0].normal_index] + up;
			this->normals[f[1].normal_index] = this->normals[f[1].normal_index] + up;
			this->normals[f[2].normal_index] = this->normals[f[2].normal_index] + up;
		}

		for (auto &n : this->normals)
			n = ::normalize(n);
	}
	else {
		int f_count = this->faces.count();
		this->normals = QVector<Normal>(f_count, Normal(0, 0, 0));
		for (int i = 0; i < f_count; i++) {
			this->faces[i][0].normal_index = i;
			this->faces[i][1].normal_index = i;
			this->faces[i][2].normal_index = i;

			Vector3f v0_v1 = this->vertexs[this->faces[i][1].vertex_index] - this->vertexs[this->faces[i][0].vertex_index];
			Vector3f v0_v2 = this->vertexs[this->faces[i][2].vertex_index] - this->vertexs[this->faces[i][0].vertex_index];

			Vector3f up = ::normalize(cross(v0_v1, v0_v2));
			this->normals[i] = up;
		}
	}
}

void Mesh::sphere_texture_mapping() {
	this->textures.clear();
	for (auto &v : vertexs) {
		Vector3f v_on_sphere = v;
		double vx = v_on_sphere.x();
		double vy = v_on_sphere.y();
		double vz = v_on_sphere.z();
		double r = sqrt(vx * vx + vy * vy + vz * vz);
		
		double u = atan(vy / vx) / M_PI + 0.5;
		double v = asin(vz / r) / M_PI + 0.5;
		this->textures.push_back(Vector2f(u, v));
	}

	for (auto &f : faces) {
		f[0].texture_index = f[0].vertex_index;
		f[1].texture_index = f[1].vertex_index;
		f[2].texture_index = f[2].vertex_index;
	}
}

void Mesh::load_mesh_from_ply() {
	QFile f(this->path);

	if (!f.exists()) {
		return;
	}

	f.open(QIODevice::ReadOnly);

	int vertex_count;
	int face_count;

	while (!f.atEnd()) {
		QString str = f.readLine();

		if (str.startsWith("element vertex")) {
			QStringList t = str.split(' ');
			vertex_count = t.back().toInt();
		}
		else if (str.startsWith("element face")) {
			QStringList t = str.split(' ');
			face_count = t.back().toInt();
		}
		else if (str.startsWith("end_header")) {
			break;
		}
		else {

		}
	}

	QString tmp;
	for (int i = 0; i < vertex_count; i++) {
		tmp = f.readLine();
		QStringList t = tmp.split(' ');

		this->insert_point(t[0].toDouble(), t[1].toDouble(), t[2].toDouble());
	}

	for (int i = 0; i < face_count; i++) {
		tmp = f.readLine();

		QStringList t = tmp.split(' ');

		int vs = t[0].toInt();
		QVector<int> vertex_index;
		for (int j = 0; j < vs; j++) {
			vertex_index.push_back(t[j + 1].toInt());
		}

		this->insert_face(vertex_index);
	}

	f.close();
}

void Mesh::load_mesh_from_obj() {
	QFile file(this->path);

	if (!file.exists()) {
		return;
	}

	file.open(QIODevice::ReadOnly);

	QStringList list;
	QString split = ' ';
	QString current_material_name;

	float max_x = -10000000, min_x = 10000000;
	float max_y = -10000000, min_y = 10000000;
	float max_z = -10000000, min_z = 10000000;

	while (!file.atEnd()) {
		QByteArray line = file.readLine();
		QString str(line);
		if (str.length() < 2)//太短~
			continue;
		if (str[0] == 'm')
		{
			QStringList str0 = str.split(' ');
			QString mtlname = str0[1];
			mtlname = mtlname.trimmed();//一定要trimmed处理，否则路径有问题
										//dirPath.append(mtlname);
										//model->mtllibName = dirPath;
										//_glReadMTL(model, model->mtllibName);
		}
		else if (str[0] == 'v') {
			if (str[1] == 't') {//纹理
				list = str.split(split);//无论是否包括Z方向的纹理都先取前两个值
				float vu = list[1].toFloat();
				float vt = list[2].toFloat();
				this->insert_texture(vu, vt);
			}
			else if (str[1] == 'n') {//法向量
				list = str.split(split);
				float vx = list[1].toFloat();
				float vy = list[2].toFloat();
				float vz = list[3].toFloat();

				this->insert_normal(vx, vy, vz);
			}
			else//节点~
			{
				list = str.split(split);
				float vx = list[1].toFloat();
				float vy = list[2].toFloat();
				float vz = list[3].toFloat();

				this->insert_point(vx, vy, vz);

				if (vx > max_x) max_x = vx;
				if (vx < min_x) min_x = vx;
				if (vy > max_y) max_y = vy;
				if (vy < min_y) min_y = vy;
				if (vz > max_z) max_z = vz;
				if (vz < min_z) min_z = vz;
			}
		}
		else if (str[0] == 'u')//材质的名称
		{
			list = str.split(split);
			current_material_name = list[1];
		}
		else if (str[0] == 'f')//面
		{
			str = str.trimmed();
			list = str.split(split);

			if (list[1].contains('/'))
			{
				QVector<FaceIndex> current_face;
				for (int i = 1; i < list.length(); i++)
				{
					QStringList sublist = list[i].split('/');
					int ix = sublist[0].toInt() - 1;
					int iy = sublist[1].toInt() - 1;
					int iz = -1;
					if (list[1].split('/').length() == 3)//只有v和vt
					{
						iz = sublist[2].toInt() - 1;
					}

					current_face.push_back(FaceIndex(ix, iy, iz));
				}

				this->insert_face(current_face);
			}
			else//不包括/，那么只有节点
			{
				QVector<int> current_face;
				for (int i = 1; i < list.length(); i++)
				{
					int ix = list[i].toInt() - 1;
					current_face.push_back(ix);
				}

				this->insert_face(current_face);
			}
		}
	}

	this->max[0] = max_x;
	this->max[1] = max_y;
	this->max[2] = max_z;

	this->min[0] = min_x;
	this->min[1] = min_y;
	this->min[2] = min_z;
}