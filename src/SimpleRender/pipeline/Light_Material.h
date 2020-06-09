#pragma once
#include "../math/Vector3f.h"
#include "../math/Vector4f.h"

class Material {
public:
	Vector3f m_ambient;
	Vector3f m_diffuse;
	Vector3f m_specular;
	double m_shiness;
	Material(const Vector3f m_a, const Vector3f m_d, const Vector3f m_s, const double m_shi) {
		load(m_a, m_d, m_s, m_shi);
	}
	inline void load(const Vector3f m_a, const Vector3f m_d, const Vector3f m_s, const double m_shi) {
		m_ambient = m_a;
		m_diffuse = m_d;
		m_specular = m_s;
		m_shiness = m_shi;
	}
};

class Light {
public:
	virtual void phong_lighting(
		const Material &material,
		const Vector3f &pos_world,
		const Vector3f &normal_world,
		const Vector3f &eye_pos,
		const Vector4f &in_color,
		Vector4f &out_color
	) = 0;
};

class DirectionalLight : public Light {
public:
	Vector3f Ip;
	Vector3f Ia;
	Vector3f direction;
	Vector3f L;
public:
	DirectionalLight(const Vector3f &_direction, const Vector3f &_ip, const Vector3f &_ia) {
		set_directional_light(_direction, _ip, _ia);
	}
	inline void set_directional_light(const Vector3f &_direction, const Vector3f &_ip, const Vector3f &_ia) {
		direction = normalize(_direction);
		Ip = _ip;
		Ia = _ia;

		L = direction * (-1.0f);
	}
	virtual void phong_lighting(
		const Material &material,
		const Vector3f &pos_world,
		const Vector3f &normal_world,
		const Vector3f &eye_pos,
		const Vector4f &in_color,
		Vector4f &out_color
	);
};

class PointLight : public Light {
public:
	Vector3f Ip;
	Vector3f Ia;
	Vector3f position;
	Vector3f attenuation;	// 
public:
	PointLight(const Vector3f &_pos, const Vector3f &_ip, const Vector3f &_ia, const Vector3f &_atte) {
		set_point_light(_pos, _ip, _ia, _atte);
	}
	inline void set_point_light(const Vector3f &_pos, const Vector3f &_ip, const Vector3f &_ia, const Vector3f &_atte) {
		position = _pos;
		Ip = _ip;
		Ia = _ia;
		attenuation = _atte;
	}
	virtual void phong_lighting(
		const Material &material,
		const Vector3f &pos_world,
		const Vector3f &normal_world,
		const Vector3f &eye_pos,
		const Vector4f &in_color,
		Vector4f &out_color
	);
};

