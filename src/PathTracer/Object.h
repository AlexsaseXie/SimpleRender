#pragma once
#include "Vector3f.h"
#include "Ray.h"
#include "Mesh.h"
#include "Material.h"

#define EPS 1e-5

class Object {
public:
	Material * material;			// material
	Vector3f e, c;					// emission, color 
	virtual double intersect(const Ray &r) = 0;
	virtual Vector3f get_normal(const Vector3f &intersection) = 0;
};

class Sphere : public Object {
public:
	double rad;       // radius 
	Vector3f p;		  // position
public:
	Sphere(const double rad_, const Vector3f p_, const Vector3f e_, const Vector3f c_, const MaterialType material_, const double refr_rate_ = 1.5) :
		rad(rad_), p(p_) {
		e = e_;
		c = c_;
		if (material_ == MaterialType::REFR)
			material = new Ideal_Refraction_Material(refr_rate_);
		else if (material_ == MaterialType::DIFF)
			material = new Blend_Diffusion_Specular_Material(1.0, 0.0);
		else // material == MaterialType::Spec
			material = new Blend_Diffusion_Specular_Material(0.0, 1.0);
	}
	virtual double intersect(const Ray &r) { // returns distance, 0 if nohit 
		Vector3f op = p - r.o; // Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0 
		double t, eps = 1e-4;
		double b = dot(op, r.d); 
		double det = b * b - dot(op, op) + rad * rad;
		if ( det < 0 ) return 0; else det = sqrt(det);
		return (t = b - det)>eps ? t : ((t = b + det)>eps ? t : 0);
	}
	virtual Vector3f get_normal(const Vector3f &intersection) { 
		return normalize(intersection - p);
	}
};



class MeshObject : public Object, public Mesh {
public:
	Vector3f center;
private:
	int intersection_face_id;
public:
	MeshObject(const QString file, const Vector3f &center, const double s, const Vector3f e_, const Vector3f c_, const MaterialType material_, const double refr_rate_ = 1.5) {
		e = e_;
		c = c_;
		if (material_ == MaterialType::REFR)
			material = new Ideal_Refraction_Material(refr_rate_);
		else if (material_ == MaterialType::DIFF)
			material = new Blend_Diffusion_Specular_Material(1.0, 0.0);
		else // material == MaterialType::Spec
			material = new Blend_Diffusion_Specular_Material(0.0, 1.0);

		this->load_mesh_from_file(file);
		this->normalize();
		float new_min[3] = {
			center.data[0] + this->min[0] * s,
			center.data[1] + this->min[1] * s ,
			center.data[2] + this->min[2] * s
		};

		float new_max[3] = {
			center.data[0] + this->max[0] * s,
			center.data[1] + this->max[1] * s,
			center.data[2] + this->max[2] * s
		};
		this->normalize(new_min, new_max);
		this->calc_normals(false);
		this->center = center;
	}
	bool to_left(const Point3 &v1, const Point3 &v2, const Point3 &to_test, const Normal &n) {
		Vector3f v2_v1 = v2 - v1;
		Vector3f v3_v1 = to_test - v1;
		Vector3f nv = cross(v2_v1, v3_v1);
		return dot(n, nv) >= 0;
	};
	bool intersect_with_aabb(const Ray &r) {
		Vector3f ptOnPlane; // intersection pt
		Vector3f min = Vector3f(this->min[0], this->min[1], this->min[2]);
		Vector3f max = Vector3f(this->max[0], this->max[1], this->max[2]);

		Vector3f origin = r.o;
		Vector3f dir = r.d; 
		float t;
		// x directions
		if (dir.x() != 0.) {
			if (dir.x() > 0)
				t = (min.x() - origin.x()) / dir.x();
			else 
				t = (max.x() - origin.x()) / dir.x();

			if (t > 0.) {
				ptOnPlane = origin + dir * t;
				if (min.y() < ptOnPlane.y() && ptOnPlane.y() < max.y() &&
					min.z() < ptOnPlane.z() && ptOnPlane.z() < max.z()) {
					return true;
				}
			}
		}
		// y directions
		if (dir.y() != 0.) {
			if (dir.y() > 0)
				t = (min.y() - origin.y()) / dir.y();
			else
				t = (max.y() - origin.y()) / dir.y();

			if (t > 0.) {
				ptOnPlane = origin + dir * t;
				if (min.x() < ptOnPlane.x() && ptOnPlane.x() < max.x() && 
					min.z() < ptOnPlane.z() && ptOnPlane.z() < max.z()) {
					return true;
				}
			}
		}
		// z directions
		if (dir.z() != 0.) {
			if (dir.z() > 0)
				t = (min.z() - origin.z()) / dir.z();
			else
				t = (max.z() - origin.z()) / dir.z();

			if (t > 0.) {
				ptOnPlane = origin + dir * t;
				if (min.x() < ptOnPlane.x() && ptOnPlane.x() < max.x() &&
					min.y() < ptOnPlane.y() && ptOnPlane.y() < max.y()) {
					return true;
				}
			}
		}
		return false;
	}
	virtual double intersect(const Ray &r) { // returns distance, 0 if nohit 
		//todo: check with the bounding box
		// check with aabb
		if (!intersect_with_aabb(r))
			return 0;
		
		const double inf = 1e20;
		double t_min = inf;
		for (int i = 0; i < this->faces.size(); i++) {
			Normal n = this->normals[faces[i][0].normal_index];
			Point3 v1 = this->vertexs[faces[i][0].vertex_index];
			double judge = dot(r.d, n);
			if (abs(judge) <= EPS)
				continue;
			double k = - dot(r.o - v1, n) / judge;
			if (k <= EPS)
				continue;

			Point3 v2 = this->vertexs[faces[i][1].vertex_index];
			Point3 v3 = this->vertexs[faces[i][2].vertex_index];
			Point3 intersection = r.o + r.d * k;
			if (to_left(v1, v2, intersection, n) 
				&& to_left(v2, v3, intersection, n) 
				&& to_left(v3, v1, intersection, n)) {
				if (k < t_min) {
					t_min = k;
					intersection_face_id = i;
				}
			}
		}

		if (t_min == inf)
			return 0;
		else
			return t_min;
	}
	virtual Vector3f get_normal(const Vector3f &intersection) {
		return this->normals[this->faces[intersection_face_id][0].normal_index];
	}
};