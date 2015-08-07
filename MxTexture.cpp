#include <MxTexture.hpp>
#include <MxVideoSurface.hpp>

#include <QDebug>
#include <QImage>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include <QOpenGLWidget>
#include <QOpenGLTexture>

QStringList MxTexture::IMAGE_SUFFIXES =
		QStringList() << "jpg"
			      << "jpeg"
			      << "png"
			      << "tga"
			      << "bmp";

QStringList MxTexture::VIDEO_SUFFIXES =
		QStringList() << "mpg"
			      << "mpeg"
			      << "mp4"
			      << "avi"
			      << "mkv";

MxTexture::MxTexture(QOpenGLWidget *widget, const QString &filePath, QObject *parent) :
	QObject(parent),
	mOpenGLTexture(nullptr),
	mOpenGLWidget(widget)
{
	if (!mOpenGLWidget) {
		fprintf(stderr, "MxTexture: empty OpenGL widget\n");
		return;
	}

	mOpenGLWidget->makeCurrent();

	if (isImage(filePath)) {
		loadImage(filePath);
	} else if (isVideo(filePath)) {
		loadVideo(filePath);
	}

	mOpenGLWidget->doneCurrent();
}

MxTexture::~MxTexture()
{
	if (mOpenGLWidget && mOpenGLTexture) {
		return;
	}

	mOpenGLWidget->makeCurrent();
	delete mOpenGLTexture;
	mOpenGLWidget->doneCurrent();
}

void MxTexture::bind()
{
	if (mOpenGLTexture) {
		mOpenGLTexture->bind();
	}
}

void MxTexture::release()
{
	if (mOpenGLTexture) {
		mOpenGLTexture->release();
	}
}

void MxTexture::loadImage(const QString &filePath)
{
	QImage image(filePath);
	if (image.isNull()) {
		fprintf(stderr, "MxTexture: failed to load texture file\n");
		return;
	}

	mOpenGLTexture = new QOpenGLTexture(image.mirrored());
	mOpenGLTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	mOpenGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
}

void MxTexture::loadVideo(const QString &filePath)
{
	auto playlist = new QMediaPlaylist;
	playlist->addMedia(QUrl::fromLocalFile(filePath));
	playlist->setCurrentIndex(0);
	playlist->setPlaybackMode(QMediaPlaylist::Loop);

	mVideoPlayer = new QMediaPlayer;
	mVideoPlayer->setPlaylist(playlist);

	mVideoSurface = new MxVideoSurface;
	mVideoPlayer->setVideoOutput(mVideoSurface);

	connect(mVideoSurface, &MxVideoSurface::onVideoFrame, this, &MxTexture::onVideoFrame);

	mVideoPlayer->play();
}

bool MxTexture::isImage(const QString &filePath)
{
	foreach (auto s, IMAGE_SUFFIXES) {
		if (filePath.endsWith(s, Qt::CaseInsensitive)) {
			return true;
		}
	}
	return false;
}

bool MxTexture::isVideo(const QString &filePath)
{
	foreach (auto s, VIDEO_SUFFIXES) {
		if (filePath.endsWith(s, Qt::CaseInsensitive)) {
			return true;
		}
	}
	return false;
}

void MxTexture::onVideoFrame(const QVideoFrame &frame)
{
	if (!mOpenGLWidget) {
		return;
	}

	mOpenGLWidget->makeCurrent();
	
	if (mOpenGLTexture && mOpenGLTexture->isCreated()) {
		QImage image(frame.bits(), frame.width(), frame.height(), QImage::Format_Grayscale8);
		mOpenGLTexture->setData(image.mirrored());
	} else {
		QImage image(frame.bits(), frame.width(), frame.height(), QImage::Format_Grayscale8);
		mOpenGLTexture = new QOpenGLTexture(image.mirrored());
		mOpenGLTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		mOpenGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	}

	mOpenGLWidget->doneCurrent();

	emit invalidate();
}
