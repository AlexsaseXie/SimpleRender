#include <omp.h>
#include <QtCore/QCoreApplication>
#include <QImage>
#include <qmath.h>
#include <iostream>
#include <random>
using namespace std;

#include "Vector3f.h"
#include "Ray.h"
#include "Object.h"
#include <time.h>

default_random_engine e;
uniform_real_distribution<double> u(0, 1);

double rand_double() {
	//return (double)rand() / (double)RAND_MAX;
	return u(e);
}

Object ** objects = nullptr;
int objects_n;

inline double clamp(double x) { 
	return qBound(0.0, x, 1.0);
}

inline int to_int(double x) { 
	return int(clamp(x) * 255.0); 
}

inline bool scene_intersect(const Ray &r, double &t, int &id) {
	int n = objects_n;
	double d, inf = t = 1e20;
	// find the nearest intersection
	for (int i = 0; i < n; i++) {
		if ((d = objects[i]->intersect(r)) && d < t) {
			t = d; id = i;
		}
	}
	return t<inf;
}

Vector3f radiance(const Ray &r, int depth) {
	double t;                               // distance to intersection 
	int id = 0;                             // id of intersected object 
	if (!scene_intersect(r, t, id)) 
		return Vector3f(); // if miss, return black 
	Object *obj = objects[id];				// the hit object 
	Vector3f intersection_point = r.o + r.d*t;
	Vector3f intersection_n = obj->get_normal(intersection_point); //normal point to outer side
	Vector3f intersection_n_same_side = dot(intersection_n, r.d) < 0 ?
		intersection_n : intersection_n * (-1.0); // normal that is on the same side of the surface
	Vector3f obj_color = obj->c;
	double p = qMax(qMax(obj_color.x(), obj_color.y()), obj_color.z()); // max refl 
	
	if (++depth > 5) {
		if (depth >= 20) // avoid stack overflow
			return obj->e;
		if (rand_double() < p)
			obj_color = obj_color * (1 / p);
		else
			return obj->e; //R.R. 
	}

	Ray wi;
	double f_r;
	double one_div_pdf_w;
	double cos_theta;

	bool flag = obj->material->sample_ray(r, intersection_point, intersection_n, wi, f_r, one_div_pdf_w, cos_theta);
	if (flag)
		return obj->e + obj_color * radiance(wi, depth) * f_r * one_div_pdf_w * cos_theta;
	else
		return obj->e;
}

void load_scene (){
	objects = new Object *[15];
	objects_n = 10;
	objects[0] = new Sphere(1e5, Vector3f(1e5 + 1, 40.8, 81.6), Vector3f(), Vector3f(.75, .25, .25), DIFF, 1.0);	//Left 
	objects[1] = new Sphere(1e5, Vector3f(-1e5 + 99, 40.8, 81.6), Vector3f(), Vector3f(.25, .25, .75), DIFF, 1.0);	//Rght 
	objects[2] = new Sphere(1e5, Vector3f(50, 40.8, 1e5), Vector3f(), Vector3f(.75, .75, .75), DIFF, 1.0);			//Back 
	objects[3] = new Sphere(1e5, Vector3f(50, 40.8, -1e5 + 170), Vector3f(), Vector3f(), DIFF, 1.0);				//Frnt 
	objects[4] = new Sphere(1e5, Vector3f(50, 1e5, 81.6), Vector3f(), Vector3f(.75, .75, .75), DIFF, 1.0);			//Botm 
	objects[5] = new Sphere(1e5, Vector3f(50, -1e5 + 81.6, 81.6), Vector3f(), Vector3f(.75, .75, .75), DIFF, 1.0);	//Top 
	objects[6] = new Sphere(16.5, Vector3f(27, 16.5, 47), Vector3f(), Vector3f(1, 1, 1)*.999, SPEC, 1.0);			//Mirr 
	objects[7] = new Sphere(16.5, Vector3f(73, 16.5, 78), Vector3f(), Vector3f(1, 1, 1)*.999, REFR, 1.5);			//Glas 
	objects[8] = new Sphere(600, Vector3f(50, 681.6 - .27, 81.6), Vector3f(80, 80, 80), Vector3f(), DIFF, 1.0);		//Lite 
	objects[9] = new MeshObject("data/cube.obj", Vector3f(25, 10, 100), 20, Vector3f(), Vector3f(0., 0.7, 0.0), DIFF, 1.0); // Box
}

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);
	// load scene
	load_scene();

	int samps = 40; // # samples
	if (argc >= 2)
		samps = atoi(argv[1]);
	int w = 800, h = 600; 
	Ray cam(Vector3f(50, 52, 295.6), normalize(Vector3f(0, -0.042612, -1))); // cam pos, dir 
	Vector3f cx = Vector3f(w *.5135 / h, 0, 0); 
	Vector3f cy = normalize(cross(cx, cam.d)) * .5135;
	Vector3f r;
	Vector3f *result_color = new Vector3f[w*h];
	int start_time = clock();
#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP 
	for (int y = 0; y<h; y++) {                       // Loop over image rows 
		fprintf(stderr, "\rRendering (%d spp) %5.2f%%", samps * 4, 100.* y / (h - 1));
		//srand(y * y * y);
		e.seed(y * y * y);
		for (unsigned short x = 0; x<w; x++)   // Loop cols 
			for (int sy = 0, i = (h - y - 1)*w + x; sy<2; sy++)     // 2x2 subpixel rows 
				for (int sx = 0; sx < 2; sx++, r = Vector3f()) {        // 2x2 subpixel cols 
					for (int s = 0; s<samps; s++) {
						double r1 = 2 * rand_double(), dx = r1<1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
						double r2 = 2 * rand_double(), dy = r2<1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
						Vector3f d = cx * (((sx + .5 + dx) / 2 + x) / w - .5) +
							cy * (((sy + .5 + dy) / 2 + y) / h - .5) + cam.d;
						r = r + radiance(Ray(cam.o + d * 140, normalize(d)), 0 ) * (1. / samps);
					} // Camera rays are pushed ^^^^^ forward to start in interior 
					result_color[i] = result_color[i] + Vector3f(clamp(r.x()), clamp(r.y()), clamp(r.z()))*.25;
				}
	}

	QImage *img = new QImage(w, h, QImage::Format::Format_RGB888);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int i = y * w + x;
			img->setPixelColor(x, y, QColor(
				to_int(result_color[i].x()), 
				to_int(result_color[i].y()), 
				to_int(result_color[i].z()) 
			));
		}
	}

	img->save("out/image.png");
	int end_time = clock();
	cout << endl << "finished in " << (double)(end_time - start_time) / (double) CLOCKS_PER_SEC << " secs" << endl;
	getchar();
	return 0;
}
