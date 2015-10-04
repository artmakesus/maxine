#include <MxTexture.hpp>
#include <MxVideoSurface.hpp>
#include <MxWebView.hpp>

#include <QDebug>
#include <QImage>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QSharedMemory>
#include <QWebFrame>
#include <QWebElement>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

QStringList MxTexture::IMAGE_SUFFIXES =
		QStringList() << "jpg"
			      << "jpeg"
			      << "png"
			      << "tga"
			      << "bmp"
			      << "webp";

QStringList MxTexture::VIDEO_SUFFIXES =
		QStringList() << "mpg"
			      << "mpeg"
			      << "mp4"
			      << "avi"
			      << "ogv"
			      << "mkv"
			      << "webm";

MxTexture::MxTexture(QOpenGLWidget *widget, int id, int width, int height, QObject *parent) :
	QObject(parent),
	mOpenGLWidget(widget),
	mOpenGLTexture(nullptr),
	mWebView(nullptr),
	mSharedFileDescriptor(-1),
	mSharedMemory(nullptr)
{
	createSharedTexture(id, width, height);
}

MxTexture::MxTexture(QOpenGLWidget *widget, const QString &filePath, QObject *parent) :
	QObject(parent),
	mOpenGLWidget(widget),
	mOpenGLTexture(nullptr),
	mWebView(nullptr),
	mSharedFileDescriptor(-1),
	mSharedMemory(nullptr)
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
	if (mOpenGLTexture) {
		mOpenGLWidget->makeCurrent();
		delete mOpenGLTexture;
		mOpenGLWidget->doneCurrent();
	}

	if (mWebView) {
		delete mWebView;
	}
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

void MxTexture::createSharedTexture(int id, int width, int height)
{
	char str[64];
	sprintf(str, "%d", id);
	mSharedFileDescriptor = shm_open(str, O_CREAT | O_RDWR, 0600);
	if (mSharedFileDescriptor == -1) {
		return;
	}

	if (posix_fallocate(mSharedFileDescriptor, 0, width * height * 4) != 0) {
		shm_unlink(str);
		return;
	}

	mSharedMemory = (int *) mmap(NULL, width * height * 4, PROT_READ | PROT_WRITE, MAP_SHARED, mSharedFileDescriptor, 0);
	mSharedTextureWidth = width;
	mSharedTextureHeight = height;
}

void MxTexture::destroySharedTexture()
{
	if (mSharedMemory) {
		delete mSharedMemory;
		mSharedMemory = nullptr;
	}

	if (mSharedFileDescriptor >= 0) {
		close(mSharedFileDescriptor);
	}
}

bool MxTexture::invalidateSharedTexture()
{
	if (!mSharedMemory) {
		return false;
	}

	auto data = (uchar *) (mSharedMemory);
	QImage image(data, mSharedTextureWidth, mSharedTextureHeight, QImage::Format_ARGB32);

	mOpenGLWidget->makeCurrent();

	if (mOpenGLTexture) {
		mOpenGLTexture->destroy();
		mOpenGLTexture->create();
		mOpenGLTexture->setData(image.mirrored());
	} else {
		mOpenGLTexture = new QOpenGLTexture(image.mirrored());
		mOpenGLTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		mOpenGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	}

	mOpenGLWidget->doneCurrent();

	emit invalidate();

	return true;
}

void MxTexture::prepareWebView()
{
	mWebView = new MxWebView;
	mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	mWebView->page()->mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
}

void MxTexture::onWebViewFrame()
{
	auto page = mWebView->page();
	auto elem = page->mainFrame()->findFirstElement("video, img");
	auto size = elem.geometry().size();
	QImage image(size, QImage::Format_ARGB32);
	if (image.isNull()) {
		return;
	}

	QPainter painter(&image);
	page->setViewportSize(size);
	elem.render(&painter);

	mOpenGLWidget->makeCurrent();
	if (mOpenGLTexture) {
		mOpenGLTexture->destroy();
		mOpenGLTexture->create();
		mOpenGLTexture->setData(image.mirrored());
	} else {
		mOpenGLTexture = new QOpenGLTexture(image.mirrored());
		mOpenGLTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		mOpenGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	}
	mOpenGLWidget->doneCurrent();

	emit invalidate();
}

void MxTexture::loadImage(const QString &filePath)
{
	prepareWebView();
	mWebView->setHtml("<html><body><img src='file://" + filePath + "' /></body></html>");
	connect(mWebView, &MxWebView::onPaint, this, &MxTexture::onWebViewFrame);
}

void MxTexture::loadVideo(const QString &filePath)
{
	prepareWebView();
	mWebView->setHtml("<html><body><video src='file://" + filePath + "' autoplay loop></video></body></html>");
	connect(mWebView, &MxWebView::onPaint, this, &MxTexture::onWebViewFrame);
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
