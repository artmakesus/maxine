#include <MxPolygonItem.hpp>
#include <MxPoint.hpp>
#include <MxScene.hpp>

#include <QPainter>
#include <QOpenGLTexture>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QtMath>

const int MxPolygonItem::CENTER = 0;
const int MxPolygonItem::FIRST = 1;

MxPolygonItem::MxPolygonItem(QGraphicsItem *parent) :
	QGraphicsPolygonItem(parent),
	mTexture(nullptr),
	mPointSelected(-1)
{
	init();

	// initial vertices
	mVertices << QPointF(25, 25)
	          << QPointF(0, 0)
	          << QPointF(50, 0)
	          << QPointF(50, 50)
	          << QPointF(0, 50)
	          << QPointF(0, 0);
	setPolygon(mVertices);

	// initial texture coordinates
	mTexCoords << QPointF(0.5, 0.5)
	           << QPointF(0, 1)
	           << QPointF(1, 1)
	           << QPointF(1, 0)
	           << QPointF(0, 0)
	           << QPointF(0, 1);
}

MxPolygonItem::MxPolygonItem(const char *texture,
                             const QPointF &pos,
							 const QPolygonF &vertices,
							 const QPolygonF &texCoords,
							 QGraphicsItem *parent) :
	QGraphicsPolygonItem(parent),
	mTexture(nullptr),
	mPointSelected(-1)
{
	init();

	if (texture) {
		mMediaFilePath = texture;
		mImage.load(texture);
	}

	// initial position
	setPos(pos);

	// initial vertices
	mVertices = vertices;
	setPolygon(mVertices);

	// initial texture coordinates
	mTexCoords = texCoords;
}

void MxPolygonItem::init() {
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	setAcceptDrops(true);
}

void MxPolygonItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
	if (mImage.isNull()) {
		drawDefault(painter);
	} else {
		drawOpenGL(painter);
	}

	auto s = static_cast<MxScene*>(scene());
	if (s->showMarkers()) {
		drawMarkers(painter);
	}
}

QPolygonF MxPolygonItem::vertices() {
	return mVertices;
}

QPolygonF MxPolygonItem::texCoords() {
	return mTexCoords;
}

QString MxPolygonItem::textureFilePath() {
	return mMediaFilePath;
}

void MxPolygonItem::dropEvent(QGraphicsSceneDragDropEvent *evt) {
	auto mimeData = evt->mimeData();
	if (mimeData->hasUrls()) {
		foreach (QUrl url, evt->mimeData()->urls()) {
			mMediaFilePath = url.path().toUtf8();
			auto s = url.path().toUtf8().data();
			mImage.load(s);
			delete mTexture;
			break;
		}
	} else if (mimeData->hasImage()) {
		mImage = qvariant_cast<QImage>(mimeData->imageData());
	}

	if (!mImage.isNull()) {
		emit invalidate();
	}
}

void MxPolygonItem::mousePressEvent(QGraphicsSceneMouseEvent *evt) {
	auto mouse = evt->pos();

	for (auto i = 0; i < mVertices.size(); i++) {
		auto p = mVertices[i];
		auto dx = p.x() - mouse.x();
		auto dy = p.y() - mouse.y();
		auto d = qSqrt(dx * dx + dy * dy);
		if (d < 8) {
			mPointSelected = i;
			return;
		}
	}

	QGraphicsPolygonItem::mousePressEvent(evt);
}

void MxPolygonItem::mouseMoveEvent(QGraphicsSceneMouseEvent *evt) {
	if (mPointSelected == -1) {
		QGraphicsPolygonItem::mouseMoveEvent(evt);
		return;
	}

	auto mouse = evt->pos();
	mVertices[mPointSelected] = mouse;
	if (mPointSelected == FIRST) {
		mVertices[mVertices.size() - 1] = mouse;
	}

	setPolygon(mVertices);
}

void MxPolygonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *evt) {
	QGraphicsPolygonItem::mouseReleaseEvent(evt);

	mPointSelected = -1;
}

void MxPolygonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *evt) {
	QGraphicsPolygonItem::mouseDoubleClickEvent(evt);

	auto mousePos = evt->pos();
	addPoint(mousePos.x(), mousePos.y());
}

void MxPolygonItem::drawDefault(QPainter *painter) {
	painter->setRenderHint(QPainter::Antialiasing);

	painter->setBrush(QBrush(QColor(0, 216, 255)));
	painter->setPen(QPen(QColor(0, 160, 240)));

	auto tmp = mVertices;
	tmp.remove(CENTER); // no need to draw the center point
	painter->drawPolygon(tmp);
}

void MxPolygonItem::drawOpenGL(QPainter *painter) {
	painter->beginNativePainting();

	glEnable(GL_MULTISAMPLE);

	if (!mTexture) {
		mTexture = new QOpenGLTexture(mImage.mirrored());
		mTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		mTexture->setMagnificationFilter(QOpenGLTexture::Linear);
	}

	mTexture->bind();

	// FIXME: using deprecated OpenGL API
	glBegin(GL_POLYGON);
	for (auto i = 0; i < mVertices.size(); i++) {
		glTexCoord2f(mTexCoords[i].x(), mTexCoords[i].y());
		glVertex2f(mVertices[i].x(), mVertices[i].y());
	}
	glEnd();

	mTexture->release();

	painter->endNativePainting();
}

void MxPolygonItem::drawMarkers(QPainter *painter) {
	painter->setBrush(QBrush(QColor(0, 0, 0)));
	painter->setPen(QPen(QColor(0, 160, 240)));

	foreach (QPointF p, mVertices) {
		painter->drawEllipse(p.x() - 4, p.y() - 4, 8, 8);
	}
}

void MxPolygonItem::addPoint(float x, float y) {
	float shortestDistance = 9999;
	int shortest = -1;

	for (auto i = 1; i <= mVertices.size(); i++) {
		auto j = (i + 1) % mVertices.size();

		// skip center point
		if (j == CENTER) {
			j = FIRST;
		}

		auto avgDistance = MxPoint::distance(QPointF(x, y), QLineF(mVertices[i], mVertices[j]));
		if (avgDistance < shortestDistance) {
			shortestDistance = avgDistance;
			shortest = i;
		}
	}

	if (shortest >= 0 && shortestDistance < 9999) {
		// add vertex
		auto i = (shortest + 1) % mVertices.size();

		// skip center point
		if (i == CENTER) {
			i = FIRST;
		}

		QPointF p(x, y);
		mVertices.insert(i, p);
		if (i == FIRST) {
			mVertices[mVertices.size() - 1] = p;
		}

		setPolygon(mVertices);

		// add texture coordinate
		auto tc = texCoordBetween(shortest, shortest + 1);
		mTexCoords.insert(i, tc);
		if (i == FIRST) {
			mTexCoords[mTexCoords.size() - 1] = tc;
		}
	}
}

QPointF MxPolygonItem::texCoordBetween(int a, int b) {
	auto ta = mTexCoords[a];
	auto tb = mTexCoords[b];
	auto tc = ta + ((tb - ta) / 2);
	return tc;
}
