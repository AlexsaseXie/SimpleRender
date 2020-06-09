#pragma once

#include <QtWidgets/QWidget>
#include "ui_SimpleRender.h"
#include "RenderLoop.h"
#include <QImage>
#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QGroupBox>
#include <QButtonGroup>

class SimpleRender : public QWidget
{
	Q_OBJECT

public:
	SimpleRender(QWidget *parent = Q_NULLPTR);
	~SimpleRender();

public:
	int width = 600;
	int height = 450;
	int num_triangles;
	int num_vertices;
	QImage * image = nullptr;
	QThread loop_thread;
	QTimer * timer;
	RenderLoop * render_loop_worker;

protected:
	// control related
	QPoint pre_mouse_pos;
	bool eye_moving = false;
	bool view_rotating = false;
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
	virtual void wheelEvent(QWheelEvent *event);

	// groups
	QButtonGroup * render_radio_group;
	QButtonGroup * projection_radio_group;


public slots:
	void show_image(unsigned char * image, int m_num_tri, int m_num_ver);
	void per_second_time_out();
	void change_render();
	void change_projection();
	void change_texture();
	
private:
	Ui::SimpleRenderClass ui;
};
