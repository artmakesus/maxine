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

	bool isMarkersShown();
	bool createSharedTexture(const QString &key, int index, int width, int height) const;
	bool invalidateSharedTexture(int index) const;

public slots:
	void addShape(MxSceneItem *shape = 0);
	void deleteShape(MxSceneItem *shape = 0);
	void toggleMarkers();

	void invalidate(const QRectF & rect = QRectF(), QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

private:
	bool mIsMarkersShown;

	MxSceneIPC *mIPC;
};

QDataStream & operator<<(QDataStream &out, const MxScene *scene);
QDataStream & operator>>(QDataStream &in, MxScene *scene);

#endif
