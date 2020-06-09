#include "Light_Material.h"
#include "../math/Utils.h"

void DirectionalLight::phong_lighting(
	const Material &material,
	const Vector3f &pos_world,
	const Vector3f &normal_world,
	const Vector3f &eye_pos,
	const Vector4f &in_color,
	Vector4f &out_color
) {
	Vector3f in_color_rgb = Vector3f(in_color.data[0], in_color.data[1], in_color.data[2]);
	// ambient
	Vector3f ambient_color = Ia * (material.m_ambient * in_color_rgb);

	// diffuse, in_color is part of Kd
	Vector3f Kd = material.m_diffuse * in_color_rgb;
	// L is calculated as -direction
	Vector3f N = normal_world;
	float L_dot_N = dot(L, N);
	Vector3f diffuse_color = Vector3f(0, 0, 0);
	Vector3f specular_color = Vector3f(0, 0, 0);

	if (L_dot_N > 0) {
		diffuse_color = Ip * Kd * L_dot_N;

		// specular
		Vector3f V = normalize(eye_pos - pos_world);
		Vector3f H = normalize(L + V);
		float H_dot_N = dot(H, N);
		if (H_dot_N > 0)
			specular_color = Ip * (material.m_specular * powf(H_dot_N, material.m_shiness));
	}
	out_color = ambient_color + diffuse_color + specular_color;
}

void PointLight::phong_lighting(
	const Material &material,
	const Vector3f &pos_world,
	const Vector3f &normal_world,
	const Vector3f &eye_pos,
	const Vector4f &in_color,
	Vector4f &out_color
) {	
	Vector3f in_color_rgb = Vector3f(in_color.data[0], in_color.data[1], in_color.data[2]);
	// ambient
	Vector3f ambient_color = Ia * material.m_ambient * in_color_rgb;

	// diffuse, in_color is part of Kd
	Vector3f Kd = material.m_diffuse * in_color_rgb;

	Vector3f L = normalize(position - pos_world);
	Vector3f N = normal_world; // already normalized
	float L_dot_N = dot(L, N);
	Vector3f diffuse_color(0, 0, 0), specular_color(0, 0, 0);
	if (L_dot_N > 0) {
		diffuse_color = Ip * Kd * qMax(0.0f, L_dot_N);

		// specular
		Vector3f V = normalize(eye_pos - pos_world);
		Vector3f H = normalize(L + V);
		float H_dot_N = dot(H, N);
		if (H_dot_N > 0)
			specular_color = Ip * (material.m_specular * powf(H_dot_N, material.m_shiness));
	}

	float distance = mol_length(position - pos_world);
	float atte = 1.0 / (
		attenuation.data[0] +
		attenuation.data[1] * distance +
		attenuation.data[2] * distance * distance
	);
	out_color = ambient_color * atte + diffuse_color * atte + specular_color * atte;
}