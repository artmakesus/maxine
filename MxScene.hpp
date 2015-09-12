#ifndef MX_SCENE_HPP
#define MX_SCENE_HPP

#include <QGraphicsScene>
#include <QDataStream>

class MxScene;
class MxSceneItem;
class MxSceneIPC;

class MxScene : public QGraphicsScene {
public:
	MxScene(QObject *parent = 0);

	void new_();
	void save(const QString &filename);
	void load(const QString &filename);
	
	void addShape(MxSceneItem *shape);

	// Are the shapes' outlines and points shown?
	bool isMarkersShown();

	// Creates shared texture of an item for image manipulations by other programs through DBus
	bool createSharedTexture(const QString &key, int index, int width, int height) const;

	// Invalidates the shared texture of an item
	bool invalidateSharedTexture(int index) const;

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

	MxSceneIPC *mIPC;
};

QDataStream & operator<<(QDataStream &out, const MxScene *scene);
QDataStream & operator>>(QDataStream &in, MxScene *scene);

#endif
