#include <MxScene.hpp>
#include <MxSceneItem.hpp>
#include <MxSceneIPC.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QDataStream>
#include <QBrush>
#include <QFile>

MxScene::MxScene(QObject *parent) :
	QGraphicsScene(parent),
	mIsMarkersShown(true)
{
	setBackgroundBrush(QBrush(QColor(0, 0, 0)));

	mIPC = new MxSceneIPC(this);
}

void MxScene::new_()
{
	// Clear all the items from the scene
	clear();
}

void MxScene::save(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}

	// Save file format keyword
	QDataStream out(&file);
	const char keyword[] = "maxine";
	out.writeRawData(keyword, 7);
	out << this;
	
	file.close();
}

void MxScene::load(const QString &filename)
{
	// Clear all the items from the scene
	clear();

	// Open file
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}
	QDataStream in(&file);

	// Load file format keyword
	char *keyword = new char[7];
	uint length = 7;
	in.readRawData(keyword, length);

	// Check if the keyword is "maxine"
	if (strcmp(keyword, "maxine") != 0) {
		fprintf(stderr, "Not a Maxine file!\n");
		delete keyword;
		return;
	}
	delete keyword;

	// Load shapes to the scene
	in >> this;
}

bool MxScene::isMarkersShown()
{
	return mIsMarkersShown;
}

bool MxScene::createSharedTexture(const QString &key, int index, int width, int height) const
{
	auto list = items();

	// Check if index is valid
	if (list.size() <= index) {
		return false;
	}

	auto item = static_cast<MxSceneItem*>(list[index]);
	return item->createSharedTexture(key, width, height);
}

bool MxScene::invalidateSharedTexture(int index) const
{
	auto list = items();

	// Check if index is valid
	if (list.size() <= index) {
		return false;
	}

	auto item = static_cast<MxSceneItem*>(list[index]);
	return item->invalidateSharedTexture();
}

void MxScene::addShape(MxSceneItem *shape)
{
	connect(shape, &MxSceneItem::invalidate,this, &MxScene::invalidate);
	addItem(shape);
}

void MxScene::addEmptyShape()
{
	auto shape = new MxSceneItem;
	shape->buildDefault();
	addShape(shape);
}

void MxScene::deleteSelectedShapes()
{
	auto items = selectedItems();
	for (auto item : items) {
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
	// Save scene resolution
	auto rect = scene->sceneRect();
	out << rect.x() << rect.y() << rect.width() << rect.height();

	// Save shapes count
	auto shapes = scene->items();
	out << (qint64) shapes.size();

	// Save shapes
	foreach (auto s, shapes) {
		out << static_cast<MxSceneItem*>(s);
	}

	return out;
}

QDataStream & operator>>(QDataStream &in, MxScene *scene)
{
	// Load scene resolution
	float x, y, width, height;
	in >> x >> y >> width >> height;
	scene->setSceneRect(x, y, width, height);

	// Load shapes count
	qint64 numShapes = 0;
	in >> numShapes;

	// Load shapes
	for (qint64 i = 0; i < numShapes; i++) {
		auto item = new MxSceneItem; 
		in >> item;
		scene->addShape(item);
	}

	return in;
}
