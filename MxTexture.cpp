#include <MxTexture.hpp>
#include <MxVideoSurface.hpp>

#include <QDebug>
#include <QImage>
#include <QMediaPlaylist>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QSharedMemory>

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

MxTexture::MxTexture(QOpenGLWidget *widget, const QString &key, int width, int height, QObject *parent) :
	QObject(parent),
	mOpenGLWidget(widget),
	mOpenGLTexture(nullptr)
{
	if (!mOpenGLWidget) {
		fprintf(stderr, "MxTexture: empty OpenGL widget\n");
		return;
	}

	mOpenGLWidget->makeCurrent();

	mSharedMemory = new QSharedMemory(key, this);
	mSharedMemory->create(width * height * 4);
	mSharedTextureWidth = width;
	mSharedTextureHeight = height;

	mOpenGLWidget->doneCurrent();
}

MxTexture::MxTexture(QOpenGLWidget *widget, const QString &filePath, QObject *parent) :
	QObject(parent),
	mOpenGLWidget(widget),
	mOpenGLTexture(nullptr)
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
	if (mVideoPlayer) {
		delete mVideoPlayer;
	}

	if (!(mOpenGLWidget && mOpenGLTexture)) {
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

bool MxTexture::invalidateSharedTexture()
{
	if (!mSharedMemory) {
		return false;
	}

	mOpenGLWidget->makeCurrent();
	mSharedMemory->lock();

	auto data = static_cast<uchar *>(mSharedMemory->data());
	QImage image(data, mSharedTextureWidth, mSharedTextureHeight, QImage::Format_ARGB32);
	mOpenGLTexture = new QOpenGLTexture(image.mirrored());
	mOpenGLTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	mOpenGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	emit invalidate();

	mSharedMemory->unlock();
	mOpenGLWidget->doneCurrent();

	return true;
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
	connect(mVideoPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(onMediaPlayerError(QMediaPlayer::Error)));

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

void MxTexture::onMediaPlayerError(QMediaPlayer::Error error)
{
	if (mVideoPlayer->state() == QMediaPlayer::PlayingState) {
		mVideoPlayer->stop();
	}

	delete mOpenGLTexture;
	// FIXME: deleting mVideoPlayer somehow crashes the program
	// delete mVideoPlayer;
}
