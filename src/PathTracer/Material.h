#pragma once
#include "Vector3f.h"
#include "Ray.h"

enum MaterialType { DIFF, SPEC, REFR };  // material types, used in radiance() 

class Material {
public:
	virtual bool sample_ray(
		const Ray &r,
		const Vector3f &intersection_point,
		const Vector3f &intersection_normal,
		Ray &wi,
		double &f_r,
		double &one_div_pdf_w,
		double &cos_theta
	 ) = 0;
};

class Blend_Diffusion_Specular_Material : public Material {
public:
	double m_diff = 0.0;
	double m_spec = 0.0;
public:
	Blend_Diffusion_Specular_Material() { m_diff = 1.0; }
	Blend_Diffusion_Specular_Material(double _diff, double _spec) { m_diff = _diff; m_spec = _spec; }
	virtual bool sample_ray(
		const Ray &r,
		const Vector3f &intersection_point,
		const Vector3f &intersection_normal,
		Ray &wi,
		double &f_r,
		double &one_div_pdf_wi,
		double &cos_theta
	);
};

class Ideal_Refraction_Material : public Material {
public:
	double refraction_rate = 1.0;
public:
	Ideal_Refraction_Material() {}
	Ideal_Refraction_Material(double r) { refraction_rate = r; }
public:
	virtual bool sample_ray(
		const Ray &r,
		const Vector3f &intersection_point,
		const Vector3f &intersection_normal,
		Ray &wi,
		double &f_r,
		double &one_div_pdf_w,
		double &cos_theta
	);
};