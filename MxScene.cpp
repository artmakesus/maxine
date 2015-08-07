#include <MxScene.hpp>
#include <MxPolygonItem.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QDataStream>
#include <QBrush>
#include <QFile>

MxScene::MxScene(QObject *parent) :
   QGraphicsScene(parent),
   mShowMarkers(true)
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
	out.setFloatingPointPrecision(QDataStream::SinglePrecision);

	out << QString("maxine");

	// save scene resolution
	auto rect = sceneRect();
	out << rect.x() << rect.y() << rect.width() << rect.height();

	// save shapes count
	auto shapes = items();
	out << (qint32) shapes.size();

	// save shapes
	foreach (auto s, shapes) {
		saveShape(out, static_cast<MxPolygonItem*>(s));
	}

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
	in.setFloatingPointPrecision(QDataStream::SinglePrecision);

	// load file format keyword
	QString keyword;
	in >> keyword;

	if (keyword != "maxine") {
		fprintf(stderr, "Not a Maxine file!\n");
		return;
	}
	
	// load scene resolution
	float x, y, width, height;
	in >> x >> y >> width >> height;
	setSceneRect(x, y, width, height);

	// load shapes count
	qint32 numShapes = 0;
	in >> numShapes;

	// load shapes
	for (qint32 i = 0; i < numShapes; i++) {
		loadShape(in);
	}
}

bool MxScene::showMarkers()
{
	return mShowMarkers;
}

void MxScene::addShape(MxPolygonItem *shape)
{
	if (!shape) {
		shape = new MxPolygonItem;
	}

	connect(shape, &MxPolygonItem::invalidate,this, &MxScene::invalidate);

	addItem(shape);
}

void MxScene::deleteShape(MxPolygonItem *shape)
{
	auto items = selectedItems();
	foreach (auto item, items) {
		removeItem(item);
	}
}

void MxScene::toggleMarkers()
{
	mShowMarkers = !mShowMarkers;
	invalidate();
}

void MxScene::invalidate(const QRectF & rect, QGraphicsScene::SceneLayers layers)
{
	QGraphicsScene::invalidate(rect, layers);
}

void MxScene::saveShape(QDataStream &out, MxPolygonItem *ss)
{
	auto media = ss->textureFilePath();

	// save texture
	out << !media.isEmpty();
	if (!media.isEmpty()) {
		out << media;
	}

	// save position
	auto p = ss->pos();
	out << p.x() << p.y();

	// save vertices count
	auto vs = ss->vertices();
	out << (qint32) vs.size();

	// save vertices
	foreach (auto v, vs) {
		out << v.x() << v.y();
	}

	// save texture coordinates count
	auto tcs = ss->texCoords();
	out << (qint32) tcs.size();

	// save texture coordinates
	foreach (auto tc, tcs) {
		out << tc.x() << tc.y();
	}
}

void MxScene::loadShape(QDataStream &in)
{
	QString textureFilePath;
	bool hasTexture = 0;
	int numVertices = 0;
	int numTexCoords = 0;
	QPolygonF vertices;
	QPolygonF texCoords;

	// load texture
	in >> hasTexture;
	if (hasTexture) {
		in >> textureFilePath;
	}

	// load position
	float x, y;
	in >> x >> y;
	QPointF pos(x, y);

	// load vertices
	in >> numVertices;
	for (qint32 i = 0; i < numVertices; i++) {
		in >> x >> y;
		vertices << QPointF(x, y);
	}

	// load texture coordinates
	in >> numTexCoords;
	for (qint32 i = 0; i < numTexCoords; i++) {
		in >> x >> y;
		texCoords << QPointF(x, y);
	}

	addShape(new MxPolygonItem(textureFilePath, pos, vertices, texCoords));
}
