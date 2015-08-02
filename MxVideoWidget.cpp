#include <MxVideoWidget.hpp>

MxVideoWidget::MxVideoWidget(QWidget *parent) :
	QVideoWidget(parent)
{
}

void MxVideoWidget::paintEvent(QPaintEvent *event)
{
	auto pixmap = grab(rect());
	emit onVideoFrame(&pixmap);
	//QVideoWidget::paintEvent(event);
}
