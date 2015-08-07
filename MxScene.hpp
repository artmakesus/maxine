#ifndef MX_SCENE_HPP
#define MX_SCENE_HPP

#include <QGraphicsScene>
#include <QDataStream>

class MxPolygonItem;

class MxScene : public QGraphicsScene {
public:
	MxScene(QObject *parent = 0);

	void new_();
	void save(const QString &filename);
	void load(const QString &filename);

	bool showMarkers();

public slots:
	void addShape(MxPolygonItem *shape = 0);
	void deleteShape(MxPolygonItem *shape = 0);
	void toggleMarkers();

	void invalidate(const QRectF & rect = QRectF(),
			QGraphicsScene::SceneLayers layers = QGraphicsScene::AllLayers);

private:
	void saveShape(QDataStream &out, MxPolygonItem *);
	void loadShape(QDataStream &in);

	bool mShowMarkers;
};

#endif
