#ifndef MX_TEXTURE_HPP
#define MX_TEXTURE_HPP

#include <QGraphicsScene>
#include <QVideoFrame>

class MxVideoSurface;
class QMediaPlayer;
class QOpenGLWidget;
class QOpenGLTexture;
class QPixmap;

class MxTexture : public QObject {
	Q_OBJECT

public:
	MxTexture(QOpenGLWidget *widget, const QString &filePath, QObject *parent = 0);
	~MxTexture();

	void bind();
	void release();

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

	void loadImage(const QString &filePath);
	void loadVideo(const QString &filePath);

	bool isImage(const QString &filePath);
	bool isVideo(const QString &filePath);

private slots:
	void onVideoFrame(const QVideoFrame &frame);
};

#endif
