#include <MxTexture.hpp>
#include <MxVideoSurface.hpp>
#include <MxWebView.hpp>

#include <QDebug>
#include <QImage>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QSharedMemory>
#include <QWebFrame>

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
			      << "ogv"
			      << "mkv";

MxTexture::MxTexture(QOpenGLWidget *widget, const QString &key, int width, int height, QObject *parent) :
	QObject(parent),
	mOpenGLWidget(widget),
	mOpenGLTexture(nullptr),
	mWebView(nullptr)
{
	if (!mOpenGLWidget) {
		fprintf(stderr, "MxTexture: empty OpenGL widget\n");
		return;
	}

	mSharedMemory = new QSharedMemory(key, this);
	mSharedMemory->create(width * height * 4);
	mSharedTextureWidth = width;
	mSharedTextureHeight = height;
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

	if (isImage(filePath)) {
		loadImage(filePath);
	} else if (isVideo(filePath)) {
		loadVideo(filePath);
	}
}

MxTexture::~MxTexture()
{
	if (!mOpenGLTexture) {
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

void MxTexture::prepareWebView()
{
	mWebView = new MxWebView;
	mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
}

void MxTexture::onFrame()
{
	auto size = mWebView->page()->mainFrame()->contentsSize();
	QImage image(size, QImage::Format_ARGB32);
	QPainter painter(&image);
	mWebView->page()->setViewportSize(size);
	mWebView->page()->mainFrame()->render(&painter);
	mOpenGLWidget->makeCurrent();
	mOpenGLTexture = new QOpenGLTexture(image.mirrored());
	mOpenGLTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	mOpenGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	mOpenGLWidget->doneCurrent();
	emit invalidate();
}

void MxTexture::loadImage(const QString &filePath)
{
	prepareWebView();
	mWebView->setHtml("<html><head><style>*{margin: 0}</style></head><body><img src='file://" + filePath + "' /></body></html>");
	connect(mWebView, &MxWebView::onPaint, this, &MxTexture::onFrame);
}

void MxTexture::loadVideo(const QString &filePath)
{
	prepareWebView();
	mWebView->setHtml("<html><head><style>*{margin: 0}</style></head><body><video src='file://" + filePath + "' autoplay loop></video></body></html>");
	connect(mWebView, &MxWebView::onPaint, this, &MxTexture::onFrame);
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
