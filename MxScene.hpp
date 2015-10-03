#ifndef MX_SCENE_HPP
#define MX_SCENE_HPP

#include <QGraphicsScene>
#include <QDataStream>

class MxScene;
class MxSceneItem;

class MxScene : public QGraphicsScene {
public:
	MxScene(QObject *parent = 0);

	void new_();
	void save(const QString &filename);
	void load(const QString &filename);
	
	void addShape(MxSceneItem *shape);
	void createSharedTexture(int id, int width, int height);
	void destroySharedTexture(int id);
	void invalidateSharedTexture(int id);

	// Are the shapes' outlines and points shown?
	bool isMarkersShown();

public slots:
	// Add empty shape
	void addEmptyShape();

	// Delete selected shapes
	void deleteSelectedShapes();

	// Toggle markers off / on
	void toggleMarkers();

	// Invalidate texture to update its display
	void invalidate(const QRectF & rect = QRectF(), QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

private:
	bool mIsMarkersShown;
};

QDataStream & operator<<(QDataStream &out, const MxScene *scene);
QDataStream & operator>>(QDataStream &in, MxScene *scene);

#endif
