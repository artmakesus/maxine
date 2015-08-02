#ifndef MX_VIDEOSURFACE_HPP
#define MX_VIDEOSURFACE_HPP

#include <QAbstractVideoSurface>

class MxVideoSurface : public QAbstractVideoSurface {
	Q_OBJECT

public:
	MxVideoSurface(QObject *parent = 0);

	bool present(const QVideoFrame &frame);
	QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type = QAbstractVideoBuffer::NoHandle) const;

signals:
	void onVideoFrame(QVideoFrame frame);
};

#endif
