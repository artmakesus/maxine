#include <MxVideoSurface.hpp>

MxVideoSurface::MxVideoSurface(QObject *parent) :
	QAbstractVideoSurface(parent)
{
}

bool MxVideoSurface::present(const QVideoFrame &frame)
{
	auto dup = frame;

	dup.map(QAbstractVideoBuffer::ReadOnly);

	emit onVideoFrame(dup);

	return true;
}

QList<QVideoFrame::PixelFormat> MxVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
	return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_RGB24;
}
