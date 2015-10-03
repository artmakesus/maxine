#ifndef MX_SCENEITEM_HPP
#define MX_SCENEITEM_HPP

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPen>
#include <QPolygonF>

class MxTexture;

class QGraphicsItem;
class QJsonArray;
class QPainter;
class QOpenGLWidget;
class QStyleOptionGraphicsItem;
class QWidget;

class MxSceneItem : public QObject, public QGraphicsItem {
	Q_OBJECT
public:
	MxSceneItem(QGraphicsItem *parent = 0);
	~MxSceneItem();

	void init();

	// Add default vertices and texture coordinates
	void buildDefault();

	// Set file path for texture that will be loaded at later time
	void setTextureFilePath(const QString &filepath);

	// Set vertices
	void setVertices(const QPolygonF &vertices);

	// Set texture coordinates
	void setTextureCoordinates(const QPolygonF &textureCoordinates);

	// Create Shared Texture
	void createSharedTexture(int width, int height);

	// Destroy Shared Texture
	void destroySharedTexture();

	// Invalidate Shared Texture
	void invalidateSharedTexture();

	int id() const;
	QPolygonF vertices() const;
	QPolygonF textureCoordinates() const;
	QString textureFilePath() const;
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

	int	   mID;
	QPolygonF  mVertices;
	QPolygonF  mTextureCoordinates;
	QString    mTextureFilePath;
	MxTexture *mTexture;
	int	   mSelectedVertex;

	void drawWithoutTexture(QPainter *painter);
	void draw(QPainter *painter);
	void drawMarkers(QPainter *painter);

	// Is texture loaded?
	bool isTextureLoaded() const;

	// Are the shape's outlines and points shown?
	bool isMarkersShown() const;

	// Is texture not loaded and texture file path
	bool shouldLoadTexture() const;

	// Add a point on the shape
	void addPoint(float x, float y);

	// Remove a point from the shape
	void removePoint(float x, float y);

	// Deselect currently selected vertex
	void deselectVertex();

	// Calculate texture coordinate between index A and B
	QPointF textureCoordinateBetween(int a, int b);

	// Get OpenGL Widget of the current QGraphicsView
	QOpenGLWidget * openGLWidget();
};

QDataStream & operator<<(QDataStream &out, MxSceneItem *item);
QDataStream & operator>>(QDataStream &in, MxSceneItem *item);

#endif
