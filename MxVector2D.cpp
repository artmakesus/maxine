#include "MxVector2D.hpp"

MxVector2D::MxVector2D(const QPointF &point) :
	QVector2D(point)
{
}

MxVector2D::MxVector2D(const QVector2D &vector) :
	QVector2D(vector.toPoint())
{
}

QVector2D MxVector2D::project(const QVector2D &p, const QVector2D &onto)
{
	qreal d = QVector2D::dotProduct(onto, onto);

	if (d > 0) {
		qreal dp = QVector2D::dotProduct(p, onto);
		return onto * (dp / d);
	}

	return onto;
}
