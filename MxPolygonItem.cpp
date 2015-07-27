#include <MxPolygonItem.hpp>
#include <MxPoint.hpp>
#include <MxScene.hpp>

#include <QPainter>
#include <QOpenGLWidget>
#include <QOpenGLTexture>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QtMath>
#include <QDebug>

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

MxPolygonItem::MxPolygonItem(const QString &texture,
                             const QPointF &pos,
							 const QPolygonF &vertices,
							 const QPolygonF &texCoords,
							 QGraphicsItem *parent) :
	QGraphicsPolygonItem(parent),
	mTexture(nullptr),
	mPointSelected(-1)
{
	init();

	if (!texture.isEmpty()) {
		mTextureFilePath = texture;
		loadTexture(texture);
	}

	// initial position
	setPos(pos);

	// initial vertices
	mVertices = vertices;
	setPolygon(mVertices);

	// initial texture coordinates
	mTexCoords = texCoords;
}

MxPolygonItem::~MxPolygonItem() {
	deleteTexture();
}

void MxPolygonItem::init() {
	setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	setAcceptDrops(true);
}

void MxPolygonItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
	if (mTextureFilePath.isNull() || mTextureFilePath.isEmpty()) {
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
	return mTextureFilePath;
}

void MxPolygonItem::dropEvent(QGraphicsSceneDragDropEvent *evt) {
	auto mimeData = evt->mimeData();
	if (mimeData->hasUrls()) {
		foreach (QUrl url, evt->mimeData()->urls()) {
			mTextureFilePath = url.path().toUtf8();
			auto s = url.path().toUtf8().data();
			loadTexture(s);
			break;
		}
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

	if (!mTexture && !mTextureFilePath.isEmpty()) {
		loadTexture(mTextureFilePath);
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

void MxPolygonItem::removePoint(float x, float y) {
	float shortestDistance = 9999;
	int shortest = -1;

	for (auto i = 1; i <= mVertices.size(); i++) {
		auto avgDistance = MxPoint::distance(QPointF(x, y), mVertices[i]);
		if (avgDistance < shortestDistance) {
			shortestDistance = avgDistance;
			shortest = i;
		}
	}

	if (shortest < 0 && shortestDistance >= 9999) {
		return;
	}

	// add vertex
	auto i = shortest;

	// skip center point
	if (i == CENTER) {
		i = FIRST;
	}

	mVertices.remove(i);
	if (i == FIRST) {
		mVertices[mVertices.size() - 1] = mVertices[FIRST];
	}

	setPolygon(mVertices);

	// add texture coordinate
	mTexCoords.remove(i);
	if (i == FIRST) {
		mTexCoords[mTexCoords.size() - 1] = mTexCoords[FIRST];
	}
}

QPointF MxPolygonItem::texCoordBetween(int a, int b) {
	auto ta = mTexCoords[a];
	auto tb = mTexCoords[b];

	if (mTexCoords.size() > 5) {
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

QOpenGLWidget *MxPolygonItem::openGLWidget() {
	auto s = scene();
	if (!s) {
		return nullptr;
	}

	auto views = s->views();
	if (views.size() == 0) {
		return nullptr;
	}

	return static_cast<QOpenGLWidget*>(views[0]->viewport());
}

void MxPolygonItem::loadTexture(const QString &filepath) {
	auto glWidget = openGLWidget();
	if (!glWidget) {
		return;
	}

	glWidget->makeCurrent();

	if (mTexture) {
		delete mTexture;
	}

	QImage image(filepath);
	mTexture = new QOpenGLTexture(image.mirrored());
	mTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	mTexture->setMagnificationFilter(QOpenGLTexture::Linear);

	glWidget->doneCurrent();
}

void MxPolygonItem::deleteTexture() {
	if (mTexture) {
		return;
	}

	auto glWidget = openGLWidget();
	glWidget->makeCurrent();
	delete mTexture;
	glWidget->doneCurrent();
}
