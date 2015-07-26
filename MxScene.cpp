#include <MxScene.hpp>
#include <MxPolygonItem.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QTextStream>
#include <QBrush>
#include <QFile>

MxScene::MxScene(QObject *parent) :
   QGraphicsScene(parent),
   mShowMarkers(true)
{
	setBackgroundBrush(QBrush(QColor(0, 0, 0)));
}

void MxScene::new_() {
	clear();
}

void MxScene::save(const QString &filename) {
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}

	// save file format keyword
	QTextStream out(&file);
	out << "maxine ";

	// save shapes count
	auto shapes = items();
	out << (int) shapes.size() << " ";

	// save shapes
	foreach (auto s, shapes) {
		saveShape(out, static_cast<MxPolygonItem*>(s));
	}

	file.close();
}

void MxScene::load(const QString &filename) {
	// remove shapes in current scene
	clear();

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}

	QString keyword;
	QTextStream in(&file);

	// load file format keyword
	in >> keyword;
	if (keyword != "maxine") {
		fprintf(stderr, "Not a Maxine file!\n");
		return;
	}

	// load shapes count
	int numShapes = 0;
	in >> numShapes;

	// load shapes
	for (int i = 0; i < numShapes; i++) {
		loadShape(in);
	}
}

bool MxScene::showMarkers() {
	return mShowMarkers;
}

void MxScene::addShape(MxPolygonItem *shape) {
	if (!shape) {
		shape = new MxPolygonItem;
	}

	connect(shape, &MxPolygonItem::invalidate,this, &MxScene::invalidate);

	addItem(shape);
}

void MxScene::deleteShape(MxPolygonItem *shape) {
	auto items = selectedItems();
	foreach (auto item, items) {
		removeItem(item);
	}
}

void MxScene::toggleMarkers() {
	mShowMarkers = !mShowMarkers;
	invalidate();
}

void MxScene::invalidate(const QRectF & rect, QGraphicsScene::SceneLayers layers) {
	QGraphicsScene::invalidate(rect, layers);
}

void MxScene::saveShape(QTextStream &out, MxPolygonItem *ss) {
	auto media = ss->textureFilePath();

	// save texture
	out << (int) (!media.isEmpty() ? 1 : 0) << " ";
	if (!media.isEmpty()) {
		out << media << " ";
	}

	// save position
	auto p = ss->pos();
	out << p.x() << " " << p.y() << " ";

	// save vertices count
	auto vs = ss->vertices();
	out << (int) vs.size() << " ";

	// save vertices
	foreach (auto v, vs) {
		out << v.x() << " " << v.y() << " ";
	}

	// save texture coordinates count
	auto tcs = ss->texCoords();
	out << (int) tcs.size() << " ";

	// save texture coordinates
	foreach (auto tc, tcs) {
		out << tc.x() << " " << tc.y() << " ";
	}
}

void MxScene::loadShape(QTextStream &in) {
	char textureFilePath[256];
	int hasTexture = 0;
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
	for (auto i = 0; i < numVertices; i++) {
		in >> x >> y;
		vertices << QPointF(x, y);
	}

	// load texture coordinates
	in >> numTexCoords;
	for (auto i = 0; i < numTexCoords; i++) {
		float x, y;
		in >> x >> y;
		texCoords << QPointF(x, y);
	}

	addShape(new MxPolygonItem(textureFilePath, pos, vertices, texCoords));
}
