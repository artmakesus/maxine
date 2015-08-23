#ifndef MX_SCENEITEM_HPP
#define MX_SCENEITEM_HPP

#include <QGraphicsScene>
#include <QAbstractGraphicsShapeItem>
#include <QPolygonF>
#include <QBrush>
#include <QPen>

class QGraphicsItem;
class QPainter;
class QOpenGLWidget;
class QStyleOptionGraphicsItem;
class QWidget;
class MxTexture;

class MxSceneItem : public QObject, public QAbstractGraphicsShapeItem {
	Q_OBJECT
public:
	MxSceneItem(QGraphicsItem *parent = 0);
	void init();
	void buildDefault();

	void setTextureFilePath(const QString &filepath);
	void setVertices(const QPolygonF &vertices);
	void setTexCoords(const QPolygonF &texCoords);
	QPolygonF vertices();
	QPolygonF texCoords();
	QString textureFilePath();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
	void invalidate(const QRectF & rect = QRectF(), QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

protected:
	void dropEvent(QGraphicsSceneDragDropEvent *evt) override;
	void mousePressEvent(QGraphicsSceneMouseEvent *evt) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *evt) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt) override;

private:
	static const QBrush Brush;
	static const QBrush MarkersBrush;
	static const QPen   Pen;
	static const QPen   MarkersPen;

	static const int CenterPoint;
	static const int FirstPoint;

	QPolygonF  mVertices;
	QPolygonF  mTexCoords;
	MxTexture *mTexture;
	QString    mTextureFilePath;
	int	   mPointSelected;

	void drawWithoutTexture(QPainter *painter);
	void draw(QPainter *painter);
	void drawMarkers(QPainter *painter);

	bool hasTexture() const;
	bool isMarkersShown() const;
	bool shouldLoadTexture() const;

	void addPoint(float x, float y);
	void removePoint(float x, float y);
	QPointF texCoordBetween(int a, int b);

	QOpenGLWidget * openGLWidget();
};

QDataStream & operator<<(QDataStream &out, MxSceneItem *item);
QDataStream & operator>>(QDataStream &in, MxSceneItem *item);

#endif
