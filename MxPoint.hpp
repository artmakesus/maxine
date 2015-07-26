#ifndef MX_POINT_HPP
#define MX_POINT_HPP

#include <QPointF>
#include <QLineF>

class MxPoint : public QPointF {
public:
	static float distance(const QPointF &p, const QPointF &q);
	static float distance(const QPointF &p, const QLineF &line);
	static bool intersectLine(const QPointF &p, const QLineF &line);
};

#endif
