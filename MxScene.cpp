#include <MxScene.hpp>
#include <MxSceneItem.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QDataStream>
#include <QBrush>
#include <QFile>

MxScene::MxScene(QObject *parent) :
   QGraphicsScene(parent),
   mIsMarkersShown(true)
{
	setBackgroundBrush(QBrush(QColor(0, 0, 0)));
}

void MxScene::new_()
{
	clear();
}

void MxScene::save(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}

	// save file format keyword
	QDataStream out(&file);
	const char keyword[] = "maxine";
	out.writeRawData(keyword, 7);
	out << this;
	
	file.close();
}

void MxScene::load(const QString &filename)
{
	clear();

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	QDataStream in(&file);

	// load file format keyword
	char *keyword = new char[7];
	uint length = 7;
	in.readRawData(keyword, length);

	if (strcmp(keyword, "maxine") != 0) {
		fprintf(stderr, "Not a Maxine file!\n");
		delete keyword;
		return;
	}
	delete keyword;

	in >> this;
}

bool MxScene::isMarkersShown()
{
	return mIsMarkersShown;
}

void MxScene::addShape(MxSceneItem *shape)
{
	if (!shape) {
		shape = new MxSceneItem;
		shape->buildDefault();
	}

	connect(shape, &MxSceneItem::invalidate,this, &MxScene::invalidate);
	addItem(shape);
}

void MxScene::deleteShape(MxSceneItem *shape)
{
	auto items = selectedItems();
	foreach (auto item, items) {
		removeItem(item);
	}
}

void MxScene::toggleMarkers()
{
	mIsMarkersShown = !mIsMarkersShown;
	invalidate();
}

void MxScene::invalidate(const QRectF & rect, QGraphicsScene::SceneLayers layers)
{
	QGraphicsScene::invalidate(rect, layers);
}

QDataStream & operator<<(QDataStream &out, const MxScene *scene)
{
	// save scene resolution
	auto rect = scene->sceneRect();
	out << rect.x() << rect.y() << rect.width() << rect.height();

	// save shapes count
	auto shapes = scene->items();
	out << (qint64) shapes.size();

	// save shapes
	foreach (auto s, shapes) {
		out << static_cast<MxSceneItem*>(s);
	}

	return out;
}

QDataStream & operator>>(QDataStream &in, MxScene *scene)
{
	// load scene resolution
	float x, y, width, height;
	in >> x >> y >> width >> height;
	scene->setSceneRect(x, y, width, height);

	// load shapes count
	qint64 numShapes = 0;
	in >> numShapes;

	// load shapes
	for (qint64 i = 0; i < numShapes; i++) {
		auto item = new MxSceneItem; 
		in >> item;
		scene->addShape(item);
	}

	return in;
}
