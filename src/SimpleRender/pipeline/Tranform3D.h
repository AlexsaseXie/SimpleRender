#pragma once
#include "../math/Vector3f.h"
#include "../math/Mat4f_Transform.h"

class Transform3D {
public:
	Vector3f t;
	Vector3f r_axis;
	float r_degree;
	Vector3f s;
public:
	Transform3D() {
		t = Vector3f(0, 0, 0);
		r_axis = Vector3f(0, 0, 1);
		r_degree = 0;
		s = Vector3f(1, 1, 1);
	}

	inline void set_translation(const Vector3f &bt) { t = bt; }
	inline void set_rotate(const Vector3f &axis, float degree) { r_axis = axis; r_degree = degree; }
	inline void set_scale(const Vector3f& bs) { s = bs; }
	
	Mat4f to_matrix() {
		Mat4f mat;
		mat.load_identity();

		if (this->t.x() != 0 || this->t.y() != 0 || this->t.z() != 0)
			mat.translation(this->t);
		if (this->r_degree != 0)
			mat.rotate(this->r_axis, this->r_degree);
		if (this->s.x() != 1 || this->s.y() != 1 || this->s.z() != 1)
			mat.scale(this->s);

		return mat;
	}

	Mat4f to_inverse_transposed_matrix() {
		Mat4f mat;
		mat.load_identity();

		if (this->s.x() != 1 || this->s.y() != 1 || this->s.z() != 1)
			// for S inverse = scale_mat(1/a,1/b,1/c)
			mat.scale(Vector3f(1.0f / s.data[0], 1.0f / s.data[1], 1.0f / s.data[2]));		
		if (this->r_degree != 0) {
			// for R inverse = transpose
			Mat4f rotate_mat = transpose(rotate_matrix(r_axis, r_degree));
			mat.tranform(rotate_mat);
		}
		// translation not matters;
		//if (this->t.x() != 0 || this->t.y() != 0 || this->t.z() != 0)
			// for T inverse = translation_mat(-a,-b,-c)
		//	mat.translation(this->t * (-1.0f));

		mat = transpose(mat);
		return mat;
	}
};
