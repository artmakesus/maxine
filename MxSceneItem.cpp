#include <MxScene.hpp>
#include <MxSceneItem.hpp>
#include <MxTexture.hpp>
#include <MxPoint.hpp>
#include <QPainter>
#include <QOpenGLWidget>
#include <QGraphicsView>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QtMath>

const QBrush MxSceneItem::Brush 	= QBrush(QColor(0, 216, 255));
const QBrush MxSceneItem::MarkersBrush 	= QBrush(QColor(0, 0, 0));
const QPen MxSceneItem::Pen 		= QPen(QColor(0, 160, 240));
const QPen MxSceneItem::MarkersPen 	= QPen(QColor(0, 160, 240));

const int MxSceneItem::CenterPoint = 0;
const int MxSceneItem::FirstPoint = 1;

MxSceneItem::MxSceneItem(QGraphicsItem *parent) :
	QAbstractGraphicsShapeItem(parent)
{
	init();
}

MxSceneItem::~MxSceneItem()
{
	if (mTexture) {
		delete mTexture;
	}
}

void MxSceneItem::init()
{
	mSelectedVertex = -1;
	mTexture = nullptr;

	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	setAcceptDrops(true);
}

void MxSceneItem::buildDefault()
{
	mVertices << QPointF(200, 200)
	          << QPointF(0, 0)
	          << QPointF(400, 0)
	          << QPointF(400, 400)
	          << QPointF(0, 400)
	          << QPointF(0, 0);

	mTextureCoordinates << QPointF(0.5, 0.5)
			    << QPointF(0, 1)
			    << QPointF(1, 1)
			    << QPointF(1, 0)
			    << QPointF(0, 0)
			    << QPointF(0, 1);
}

void MxSceneItem::setTextureFilePath(const QString &filepath)
{
	mTextureFilePath = filepath;
}

void MxSceneItem::setVertices(const QPolygonF &vertices)
{
	mVertices = vertices;
}

void MxSceneItem::setTextureCoordinates(const QPolygonF &textureCoordinates)
{
	mTextureCoordinates = textureCoordinates;
}

QPolygonF MxSceneItem::vertices() const
{
	return mVertices;
}

QPolygonF MxSceneItem::textureCoordinates() const
{
	return mTextureCoordinates;
}

QString MxSceneItem::textureFilePath() const
{
	return mTextureFilePath;
}

bool MxSceneItem::createSharedTexture(const QString &key, int width, int height)
{
	if (mTexture) {
		delete mTexture;
	}
	mTexture = new MxTexture(openGLWidget(), key, width, height, this);
	return true;
}

bool MxSceneItem::invalidateSharedTexture()
{
	if (mTexture) {
		auto b = mTexture->invalidateSharedTexture();
		emit invalidate();
		return b;
	}
	return false;
}

QRectF MxSceneItem::boundingRect() const
{
	return mVertices.boundingRect();
}

void MxSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (shouldLoadTexture() || isTextureLoaded()) {
		draw(painter);
	} else {
		drawWithoutTexture(painter);
	}

	if (isMarkersShown()) {
		drawMarkers(painter);
	}
}

void MxSceneItem::dropEvent(QGraphicsSceneDragDropEvent *evt)
{
	auto mimeData = evt->mimeData();

	if (mimeData->hasUrls()) {
		foreach (QUrl url, evt->mimeData()->urls()) {
			if (mTexture) {
				delete mTexture;
			}

			mTextureFilePath = url.path().toUtf8();
			mTexture = new MxTexture(openGLWidget(), mTextureFilePath);
			connect(mTexture, &MxTexture::invalidate, this, &MxSceneItem::invalidate);
			emit invalidate();
			break;
		}
	}
}

void MxSceneItem::mousePressEvent(QGraphicsSceneMouseEvent *evt)
{
	auto mouse = evt->pos();

	// Find which point of the item is selected
	for (auto i = 0; i < mVertices.size(); i++) {
		auto p = mVertices[i];
		auto dx = p.x() - mouse.x();
		auto dy = p.y() - mouse.y();
		auto d = qSqrt(dx * dx + dy * dy);
		if (d < 8) {
			mSelectedVertex = i;
			return;
		}
	}

	QAbstractGraphicsShapeItem::mousePressEvent(evt);
}

void MxSceneItem::mouseMoveEvent(QGraphicsSceneMouseEvent *evt)
{
	// No vertex is selected so just let parent handle it
	if (mSelectedVertex == -1) {
		QAbstractGraphicsShapeItem::mouseMoveEvent(evt);
		return;
	}

	// Move selected vertex
	auto mouse = evt->pos();
	mVertices[mSelectedVertex] = mouse;
	if (mSelectedVertex == FirstPoint) {
		mVertices[mVertices.size() - 1] = mouse;
	}

	emit invalidate();
}

void MxSceneItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt)
{
	QAbstractGraphicsShapeItem::mouseReleaseEvent(evt);

	deselectVertex();
}

void MxSceneItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt)
{
	QAbstractGraphicsShapeItem::mouseDoubleClickEvent(evt);

	auto mousePos = evt->pos();

	switch (evt->button()) {
	case Qt::LeftButton:
		addPoint(mousePos.x(), mousePos.y());
		break;
	case Qt::RightButton:
		if (mVertices.size() > 3) {
			removePoint(mousePos.x(), mousePos.y());
		}
		break;
	default:
		break;
	}
}

void MxSceneItem::drawWithoutTexture(QPainter *painter)
{
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(Brush);
	painter->setPen(Pen);
	painter->drawPolygon(mVertices);
}

void MxSceneItem::draw(QPainter *painter)
{
	painter->beginNativePainting();

	glEnable(GL_MULTISAMPLE);

	if (shouldLoadTexture()) {
		mTexture = new MxTexture(openGLWidget(), mTextureFilePath);
	}

	mTexture->bind();

	// FIXME: Using deprecated OpenGL API
	glBegin(GL_POLYGON);
	for (auto i = 0; i < mVertices.size(); i++) {
		glTexCoord2f(mTextureCoordinates[i].x(), mTextureCoordinates[i].y());
		glVertex2f(mVertices[i].x(), mVertices[i].y());
	}
	glEnd();

	mTexture->release();

	painter->endNativePainting();
}

void MxSceneItem::drawMarkers(QPainter *painter)
{
	painter->setBrush(MarkersBrush);
	painter->setPen(MarkersPen);

	foreach (QPointF p, mVertices) {
		painter->drawEllipse(p.x() - 4, p.y() - 4, 8, 8);
	}
}

bool MxSceneItem::isTextureLoaded() const
{
	return mTexture;
}

bool MxSceneItem::isMarkersShown() const
{
	auto s = static_cast<MxScene*>(scene());
	return s->isMarkersShown();
}

bool MxSceneItem::shouldLoadTexture() const
{
	return !isTextureLoaded() && !mTextureFilePath.isEmpty();
}

void MxSceneItem::addPoint(float x, float y)
{
	float shortestDistance = 9999;
	int shortest = -1;

	for (auto i = 1; i <= mVertices.size(); i++) {
		auto j = (i + 1) % mVertices.size();

		// skip center point
		if (j == CenterPoint) {
			j = FirstPoint;
		}

		QPointF point(x, y);
		QLineF line(mVertices[i], mVertices[j]);

		auto avgDistance = MxPoint::distance(point, line);
		if (avgDistance < shortestDistance) {
			shortestDistance = avgDistance;
			shortest = i;
		}
	}

	if (shortest < 0 && shortestDistance >= 9999) {
		return;
	}

	auto i = (shortest + 1) % mVertices.size();

	// Skip center point
	if (i == CenterPoint) {
		i = FirstPoint;
	}

	QPointF p(x, y);
	mVertices.insert(i, p);
	if (i == FirstPoint) {
		mVertices[mVertices.size() - 1] = p;
	}

	// Add texture coordinate
	auto tc = textureCoordinateBetween(shortest, shortest + 1);
	mTextureCoordinates.insert(i, tc);
	if (i == FirstPoint) {
		mTextureCoordinates[mTextureCoordinates.size() - 1] = tc;
	}
}

void MxSceneItem::removePoint(float x, float y)
{
	float shortestDistance = 9999;
	int shortest = -1;

	// Find the closest vertex
	for (auto i = FirstPoint; i <= mVertices.size(); i++) {
		auto avgDistance = MxPoint::distance(QPointF(x, y), mVertices[i]);
		if (avgDistance < shortestDistance) {
			shortestDistance = avgDistance;
			shortest = i;
		}
	}

	// Didn't find any closest vertex
	if (shortest < 0 && shortestDistance >= 9999) {
		return;
	}

	// Found shortest vertex
	auto i = shortest;

	// Remove the vertex
	mVertices.remove(i);

	// If removed vertex was the first vertex, assign the current first vertex to the last vertex
	if (i == FirstPoint) {
		mVertices[mVertices.size() - 1] = mVertices[FirstPoint];
	}

	// If removed vertex was the first vertex, assign the current first texture coordinate to the last texture coordinate
	mTextureCoordinates.remove(i);
	if (i == FirstPoint) {
		mTextureCoordinates[mTextureCoordinates.size() - 1] = mTextureCoordinates[FirstPoint];
	}
}

void MxSceneItem::deselectVertex()
{
	mSelectedVertex = -1;
}

QPointF MxSceneItem::textureCoordinateBetween(int a, int b)
{
	auto ta = mTextureCoordinates[a];
	auto tb = mTextureCoordinates[b];

	if (mTextureCoordinates.size() > 5) {
		auto tc = ta + ((tb - ta) / 2);
		return tc;
	}

	if (ta.x() == 0 && ta.y() == 1) {
		return QPointF(1, 1);
	} else if (ta.x() == 1 && ta.y() == 1) {
		return QPointF(1, 0);
	} else if (ta.x() == 1 && ta.y() == 0) {
		return QPointF(0, 0);
	}

	return QPointF(0, 1);
}

QOpenGLWidget *MxSceneItem::openGLWidget()
{
	auto s = scene();
	if (!s) {
		return nullptr;
	}

	auto views = s->views();
	if (views.size() == 0) {
		return nullptr;
	}

	auto widget = static_cast<QOpenGLWidget*>(views[0]->viewport());
	return widget;
}

QDataStream & operator<<(QDataStream &out, MxSceneItem *item)
{
	// Save texture
	auto media = item->textureFilePath();
	auto hasMedia = !media.isEmpty();
	out << hasMedia;
	if (hasMedia) {
		out << media;
	}

	// Save position
	auto p = item->pos();
	out << p.x() << p.y();

	// Save vertices count
	auto vs = item->vertices();
	out << (qint64) vs.size();

	// Save vertices
	foreach (auto v, vs) {
		out << v.x() << v.y();
	}

	// Save texture coordinates count
	auto tcs = item->textureCoordinates();
	out << (qint64) tcs.size();

	// Save texture coordinates
	foreach (auto tc, tcs) {
		out << tc.x() << tc.y();
	}

	return out;
}

QDataStream & operator>>(QDataStream &in, MxSceneItem *item)
{
	// Load texture
	QString textureFilePath;
	bool hasTexture = false;
	in >> hasTexture;
	if (hasTexture) {
		in >> textureFilePath;
	}
	item->setTextureFilePath(textureFilePath);

	// Load position
	float x, y;
	in >> x >> y;
	QPointF pos(x, y);
	item->setPos(pos);

	// Load vertices
	QPolygonF vertices;
	qint64 numVertices = 0;
	in >> numVertices;
	for (qint64 i = 0; i < numVertices; i++) {
		in >> x >> y;
		vertices << QPointF(x, y);
	}
	item->setVertices(vertices);

	// Load texture coordinates
	QPolygonF textureCoordinates;
	qint64 numTexturesCoordinates = 0;
	in >> numTexturesCoordinates;
	for (qint64 i = 0; i < numTexturesCoordinates; i++) {
		in >> x >> y;
		textureCoordinates << QPointF(x, y);
	}
	item->setTextureCoordinates(textureCoordinates);

	return in;
}
