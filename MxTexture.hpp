#ifndef MX_TEXTURE_HPP
#define MX_TEXTURE_HPP

#include <QGraphicsScene>
#include <QVideoFrame>

class MxVideoSurface;
class QMediaPlayer;
class QOpenGLWidget;
class QOpenGLTexture;
class QPixmap;
class QSharedMemory;

class MxTexture : public QObject {
	Q_OBJECT

public:
	MxTexture(QOpenGLWidget *widget, const QString &key, int width, int height, QObject *parent = 0);
	MxTexture(QOpenGLWidget *widget, const QString &filePath, QObject *parent = 0);
	~MxTexture();

	void bind();
	void release();

	bool invalidateSharedTexture();

signals:
	void invalidate(const QRectF & rect = QRectF(), QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

private:
	static QStringList IMAGE_SUFFIXES;
	static QStringList VIDEO_SUFFIXES;

	QOpenGLWidget *mOpenGLWidget;
	QOpenGLTexture *mOpenGLTexture;

	// for images
	QImage *mImage;

	// for videos
	QMediaPlayer *mVideoPlayer;
	MxVideoSurface *mVideoSurface;

	// for generative visuals
	QSharedMemory * mSharedMemory;
	int mSharedTextureWidth;
	int mSharedTextureHeight;

	void loadImage(const QString &filePath);
	void loadVideo(const QString &filePath);

	bool isImage(const QString &filePath);
	bool isVideo(const QString &filePath);

private slots:
	void onVideoFrame(const QVideoFrame &frame);
};

#endif
