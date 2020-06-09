#include "SimpleRender.h"
#include <QKeyEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QFileDialog>

SimpleRender::SimpleRender(QWidget *parent)
	: QWidget(parent) {
	ui.setupUi(this);
	image = nullptr;
	timer = new QTimer();
	num_vertices = 0;
	num_triangles = 0;
	connect(timer, SIGNAL(timeout()), this, SLOT(per_second_time_out()));

	render_loop_worker = new RenderLoop(width, height);
	// render loop thread
	render_loop_worker->moveToThread(&loop_thread);

	//ui.label->setScaledContents(true);
	connect(&loop_thread, &QThread::finished, render_loop_worker, &QObject::deleteLater);
	connect(&loop_thread, &QThread::started, render_loop_worker, &RenderLoop::render_loop);
	connect(render_loop_worker, SIGNAL(frameOut(unsigned char *, int, int)), this, SLOT(show_image(unsigned char *, int, int)));

	// group box
	render_radio_group = new QButtonGroup(ui.groupBox);
	render_radio_group->addButton(ui.wireRadio, 0);
	render_radio_group->addButton(ui.meshRadio, 1);
	ui.meshRadio->setChecked(true);
	connect(ui.wireRadio, SIGNAL(clicked()), this, SLOT(change_render()));
	connect(ui.meshRadio, SIGNAL(clicked()), this, SLOT(change_render()));

	projection_radio_group = new QButtonGroup(ui.groupBox_2);
	projection_radio_group->addButton(ui.perspectiveRadio, 0);
	projection_radio_group->addButton(ui.orthoRadio, 1);
	ui.meshRadio->setChecked(true);
	connect(ui.perspectiveRadio, SIGNAL(clicked()), this, SLOT(change_projection()));
	connect(ui.orthoRadio, SIGNAL(clicked()), this, SLOT(change_projection()));
	connect(ui.textureButton, SIGNAL(clicked()), this, SLOT(change_texture()));

	// begin the thread.
	loop_thread.start();
	timer->start(1000);
}

SimpleRender::~SimpleRender()
{
	render_loop_worker->stop();
	(&loop_thread)->quit();
	(&loop_thread)->wait();
	if (image) delete image;
	delete render_radio_group;
	delete projection_radio_group;
	render_loop_worker = nullptr;
	image = nullptr;
}

void SimpleRender::per_second_time_out() {
	int fps = this->render_loop_worker->get_fps();
	this->setWindowTitle(QString("Renderer: fps: %1 triangles: %2 vertices: %3")
		.arg(fps).arg(num_triangles).arg(num_vertices));
	this->render_loop_worker->set_fps_to_zero();
}

void SimpleRender::change_render() {
	int id = render_radio_group->checkedId();
	render_loop_worker->change_render(id, true);
}

void SimpleRender::change_projection() {
	int id = projection_radio_group->checkedId();
	render_loop_worker->change_projection(id, true);
}

void SimpleRender::show_image(unsigned char * m_image, int m_num_tri, int m_num_ver) {
	this->num_triangles = m_num_tri;
	this->num_vertices = m_num_ver;

	if (image) delete image;
	image = new QImage(m_image, width, height, QImage::Format::Format_RGBA8888);

	ui.label->setPixmap(QPixmap::fromImage(*image).scaled(ui.label->width(), ui.label->height()));
}

void SimpleRender::mousePressEvent(QMouseEvent *e) {
	if (e->pos().x() <= 200)
		return;

	if (e->button() == Qt::RightButton) {
		eye_moving = true;
		pre_mouse_pos = e->pos();
	}

	if (e->button() == Qt::LeftButton) {
		view_rotating = true;
		pre_mouse_pos = e->pos();
	}
}

void SimpleRender::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() == Qt::RightButton) {
		eye_moving = false;
	}
	
	if (e->button() == Qt::LeftButton) {
		view_rotating = false;
	}
}

void SimpleRender::mouseMoveEvent(QMouseEvent *e) {
	if (eye_moving) {
		int pre_x = pre_mouse_pos.x();
		int pre_y = pre_mouse_pos.y();

		QPoint current_point = e->pos();
		if (qAbs(current_point.x() - pre_x) >= 5) {
			render_loop_worker->mouse_move_horizontal(current_point.x() - pre_x);
			pre_x = current_point.x();
		}
		if (qAbs(current_point.y() - pre_y) >= 5) {
			render_loop_worker->mouse_move_vertical(current_point.y() - pre_y);
			pre_y = current_point.y();
		}
		pre_mouse_pos = QPoint(pre_x, pre_y);
	}
	
	if (view_rotating) {
		int pre_x = pre_mouse_pos.x();
		int pre_y = pre_mouse_pos.y();

		const QPoint view_center = QPoint(ui.label->x() + ui.label->width() / 2, ui.label->y() + ui.label->height() / 2);
		QPoint v1 = pre_mouse_pos - view_center;
		QPoint v2 = e->pos() - view_center;
		QPoint v = e->pos() - pre_mouse_pos;	
		int c = v1.x() * v2.y() - v1.y() * v2.x();
		if (v.manhattanLength() >= 5 && qAbs(c) > 10) {
			render_loop_worker->view_rotate(c);
			pre_mouse_pos = e->pos();
		}
	}
}

void SimpleRender::keyPressEvent(QKeyEvent *e) {
	render_loop_worker->press_key(e->key());
}

void SimpleRender::wheelEvent(QWheelEvent *e) {
	render_loop_worker->roll_wheel(e->delta());
}

void SimpleRender::change_texture() {
	QString path = QFileDialog::getOpenFileName(this, "choose a texture image.", ".", "image files(*.bmp *.png *.jpg *.jpeg)");
	render_loop_worker->change_texture(path);
}



