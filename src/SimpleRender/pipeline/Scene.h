#pragma once
#include <QVector>
#include "Mesh.h"
#include "Camera.h"
#include "Tranform3D.h"
#include "Light_Material.h"

class Scene {
public:
	QVector<Mesh> all_mesh;
	QVector<Transform3D> all_mesh_transform;
	QVector<Light *> all_lights;
	UnboundedCamera camera;
};