#ifndef MX_VECTOR2D_HPP
#define MX_VECTOR2D_HPP

#include <QVector2D>

class MxVector2D : public QVector2D {
public:
	MxVector2D(const QPointF &point);
	MxVector2D(const QVector2D &vector);

	static QVector2D project(const QVector2D &p, const QVector2D &onto);
};

#endif
