#include "MxOpenGLWidget.hpp"

MxOpenGLWidget::MxOpenGLWidget(QWidget *parent) :
	QOpenGLWidget(parent)
{
	QSurfaceFormat format;
	format.setDepthBufferSize(24);
	format.setStencilBufferSize(8);
	format.setSamples(4);
	setFormat(format);
}

void MxOpenGLWidget::initializeGL() {
	initializeOpenGLFunctions();

	glEnable(GL_TEXTURE_2D);
}

void MxOpenGLWidget::paintGL() {
	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MxOpenGLWidget::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);

}
