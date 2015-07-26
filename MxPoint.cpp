#include "MxPoint.hpp"
#include "MxVector2D.hpp"

float MxPoint::distance(const QPointF &p, const QPointF &q) {
	return (p - q).manhattanLength();
}

float MxPoint::distance(const QPointF &p, const QLineF &line) {
	auto dx = line.p1().x() - line.p2().x();
	auto dy = line.p1().y() - line.p2().y();
	auto d = dx * dx + dy * dy;

	if (d == 0) {
		return distance(p, line.p1());
	}

	auto t = QVector2D::dotProduct(QVector2D(p - line.p1()),
	                               QVector2D(line.p2() - line.p1())) / d;
	if (t < 0) {
		return distance(p, line.p1());
	} else if (t > 1) {
		return distance(p, line.p2());
	}

	auto projection = line.p1() + t * (line.p2() - line.p1());
	return distance(p, projection);
}

bool MxPoint::intersectLine(const QPointF &p, const QLineF &line) {
	MxVector2D d(line.p1() - line.p2());
	MxVector2D lp(p - line.p1());
	MxVector2D pr(MxVector2D::project(lp, d));

	return lp == pr && 
	       pr.length() <= d.length() &&
		   QVector2D::dotProduct(pr, d) >= 0;
}
