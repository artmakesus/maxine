#include <MxVideoSurface.hpp>

MxVideoSurface::MxVideoSurface(QObject *parent) :
	QAbstractVideoSurface(parent)
{
}

bool MxVideoSurface::present(const QVideoFrame &frame)
{
	if (frame.isValid()) {
		auto dup = frame;
		dup.map(QAbstractVideoBuffer::ReadOnly);
		emit onVideoFrame(dup);
		dup.unmap();
		return true;
	}

	return false;
}

QList<QVideoFrame::PixelFormat> MxVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
	return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24;
}
