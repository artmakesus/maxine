#ifndef MX_SCENE_HPP
#define MX_SCENE_HPP

#include <QGraphicsScene>
#include <QDataStream>

class MxSceneItem;
class MxScene;

class MxScene : public QGraphicsScene {
public:
	MxScene(QObject *parent = 0);

	void new_();
	void save(const QString &filename);
	void load(const QString &filename);

	bool isMarkersShown();

public slots:
	void addShape(MxSceneItem *shape = 0);
	void deleteShape(MxSceneItem *shape = 0);
	void toggleMarkers();

	void invalidate(const QRectF & rect = QRectF(), QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

private:
	void saveShape(QDataStream &out, MxSceneItem *);
	void loadShape(QDataStream &in);

	bool mIsMarkersShown;
};

QDataStream & operator<<(QDataStream &out, const MxScene *scene);
QDataStream & operator>>(QDataStream &in, MxScene *scene);

#endif
