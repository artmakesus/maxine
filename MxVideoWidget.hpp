#ifndef MX_VIDEOWIDGET_HPP
#define MX_VIDEOWIDGET_HPP

#include <QVideoWidget>

class MxVideoWidget : public QVideoWidget {
	Q_OBJECT

public:
	MxVideoWidget(QWidget *parent = 0);

signals:
	void onVideoFrame(const QPixmap *pixmap);

protected:
	void paintEvent(QPaintEvent *event) override;
};

#endif
