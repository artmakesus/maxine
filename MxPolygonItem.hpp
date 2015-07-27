#ifndef MX_POLYGONITEM_HPP
#define MX_POLYGONITEM_HPP

#include <QGraphicsScene>
#include <QGraphicsPolygonItem>

class QOpenGLWidget;
class QOpenGLTexture;

class MxPolygonItem : public QObject, public QGraphicsPolygonItem {
	Q_OBJECT
public:
	MxPolygonItem(QGraphicsItem *parent = 0);
	MxPolygonItem(const QString &texture,
	              const QPointF &pos,
	              const QPolygonF &vertices,
				  const QPolygonF &texCoords,
				  QGraphicsItem *parent = 0);
	~MxPolygonItem();

	virtual void paint(QPainter *painter,
	                   const QStyleOptionGraphicsItem *option,
	                   QWidget *widget = 0)
					   override;

	QPolygonF vertices();
	QPolygonF texCoords();
	QString textureFilePath();

signals:
	void invalidate(const QRectF & rect = QRectF(),
	                QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

protected:
	void dropEvent(QGraphicsSceneDragDropEvent *evt) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *evt) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *evt) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt) override;

private:
	void init();
	void drawDefault(QPainter *painter);
	void drawOpenGL(QPainter *painter);
	void drawMarkers(QPainter *painter);
	void addPoint(float x, float y);
	QPointF texCoordBetween(int a, int b);

	QOpenGLWidget * openGLWidget();
	void loadTexture(const QString &filepath);
	void deleteTexture();

	QString mTextureFilePath;
	QOpenGLTexture *mTexture;
	QPolygonF mVertices;
	QPolygonF mTexCoords;

	int mPointSelected;

	static const int CENTER;
	static const int FIRST;
};

#endif
