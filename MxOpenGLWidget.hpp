#ifndef MX_OPENGLWIDGET_HPP
#define MX_OPENGLWIDGET_HPP

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class MxOpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
	MxOpenGLWidget(QWidget *parent = 0);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
};

#endif
