#include "Material.h"
#include <qmath.h>
#include <cstdlib>
#include <random>
using namespace std;

//default_random_engine e;
//uniform_real_distribution<double> u(0, 1);

extern default_random_engine e;
extern uniform_real_distribution<double> u;
extern double rand_double();

bool Blend_Diffusion_Specular_Material::sample_ray(
	const Ray &r,
	const Vector3f &intersection_point,
	const Vector3f &intersection_n,
	Ray &wi,
	double &f_r,
	double &one_div_pdf_w,
	double &cos_wi
) {
	Vector3f intersection_n_same_side = dot(intersection_n, r.d) < 0 ?
		intersection_n : intersection_n * (-1.0);

	double Xi = rand_double();
	if (Xi <= m_diff) {
		// ideal diffuse
		Vector3f w = intersection_n_same_side;
		Vector3f u = normalize(cross((fabs(w.x()) > .1 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0)), w));
		Vector3f v = cross(w, u);
		// choose a random direction
		double theta = 2 * M_PI * rand_double();
		double r2 = rand_double();
		double r2s = sqrt(r2);
		Vector3f d = normalize(u * cos(theta) * r2s + v * sin(theta) * r2s + w * sqrt(1 - r2));
		double cos_w_d = dot(w, d);
		// assign for the references
		wi = Ray(intersection_point, d);
		f_r = 1.0 / (2.0 * M_PI);
		one_div_pdf_w = 2.0 * M_PI / m_diff;
		cos_wi = cos_w_d;
		return true;
	}
	else if (Xi > m_diff && Xi <= m_diff + m_spec) {
		// ideal specular
		Vector3f d = r.d - intersection_n * 2 * dot(intersection_n, r.d);
		double cos_w_d = dot(intersection_n_same_side, d);
		// assign for the references
		wi = Ray(intersection_point, d);
		// f_r = 1.0;
		f_r = 1.0 / cos_w_d;	// ideal reflection
		one_div_pdf_w = 1.0 / m_spec;
		cos_wi = cos_w_d;
		return true;
	}
	else
		return false;
}

bool Ideal_Refraction_Material::sample_ray(
	const Ray &r,
	const Vector3f &intersection_point,
	const Vector3f &intersection_n,
	Ray &wi,
	double &f_r,
	double &one_div_pdf_w,
	double &cos_wi
) {
	Vector3f intersection_n_same_side = dot(intersection_n, r.d) < 0 ?
		intersection_n : intersection_n * (-1.0);

	// Ideal dielectric REFRACTION 
	// f_refraciton = 1.0
	Ray refl_ray(intersection_point, r.d - intersection_n * 2 * dot(intersection_n, r.d));	// reflection ray direction
	bool into = dot(intersection_n, intersection_n_same_side) > 0;                // Ray from outside going in? 
	double nc = 1.0; // vacuum's refraction rate
	double nt = this->refraction_rate;
	double n1_div_n2 = into ? nc / nt : nt / nc;
	double cos_theta1 = dot(r.d, intersection_n_same_side);
	double cos_2_theta2 = 1 - n1_div_n2 * n1_div_n2 * (1 - cos_theta1 * cos_theta1);

	double cos_refl_theta = dot(into ? intersection_n : intersection_n * (-1.0), refl_ray.d);
	if (cos_2_theta2 < 0) {    
		// Total internal reflection 
		wi = refl_ray;
		//f_r = 1.0;
		f_r = 1.0 / cos_refl_theta; // ideal reflection
		one_div_pdf_w = 1.0;
		cos_wi = cos_refl_theta;
		return true;
	}

	double cos_theta2 = sqrt(cos_2_theta2);
	// calculate the refraction ray
	Ray refr_ray = Ray( intersection_point, normalize(
		r.d * n1_div_n2 - intersection_n * ((into ? 1 : -1) * (cos_theta1 * n1_div_n2 + cos_theta2))
	));

	double a = nt - nc;
	double b = nt + nc;
	double R0 = a * a / (b * b);
	double c = 1 - (into ? - cos_theta1 : dot(refr_ray.d, intersection_n));
	double Re = R0 + (1 - R0) * pow(c, 5.0);
	double Tr = 1 - Re;
	double P = .25 + .5 * Re;
	//double RP = Re / P;
	//double TP = Tr / (1 - P);

	double cos_refr_theta = dot(into ? intersection_n * (-1.0) : intersection_n, refr_ray.d);

	// R. R to determine trace reflection ray or refraction ray
	if (rand_double() < P) {
		wi = refl_ray;
		// f_r = Re;
		f_r = Re / cos_refl_theta; 
		one_div_pdf_w = 1.0 / P;
		cos_wi = cos_refl_theta;
	}
	else {
		wi = refr_ray;
		f_r = Tr; 
		one_div_pdf_w = 1.0 / (1.0 - P);
		//cos_wi = cos_refr_theta;
		cos_wi = 1.0;
	}

	return true;
}